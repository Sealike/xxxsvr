// AppServer.cpp : 定义控制台应用程序的入口点。
//
#include "./mio/mio.h"
#include "client.h"
#include "p2p.h"
#include "tool.h"
#include "p2pclient.h"
#include <time.h>
#include "protobuffer.h"
#include "hiredis.h"
#include "frontsvr_expat.h"
#include "frontsvr_callback.h"
#include "dbaccess.h"
#include "json/cJSON.h"
#include "subscribe.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define _MAC_WIN_WSA_START(errcode_rtn) \
{\
	WORD wVersionRequested;\
	WSADATA wsaData;\
	int err;\
	wVersionRequested = MAKEWORD( 2, 2 );\
	err = WSAStartup( wVersionRequested, &wsaData );\
	if ( err != 0 ) {\
		return errcode_rtn;\
	}\
}

#define SUB_DB_MAX		30

// 唯一的应用程序对象
xht g_xsubbytaskid;
xht g_xtaskidexecutor;
int g_shutdown = 0;
int g_seqno2frontsvr = 0;


redisContext* g_rediscontext = NULL;
static void _sensor_signal(int signum)
{
	g_shutdown = 1;
}

static int init_database(_s_client_p client);

static void _save_deadsession(int waits)
{
	int count = 0;
	int i = 0;
	time_t now = time(0);
	static time_t last = 0;

	if(last == 0) last = now;

	if(now - last < waits) return ;

	last = now;

	count = jqueue_size(client()->dead_sess);

	for(;i < count ;i++)
	{
		sess_t dead = (sess_t)jqueue_pull(client()->dead_sess);		
		if (dead->type == stream_SESS){
			p2pclient_t p2p = (p2pclient_t)dead;
			BOOL connected = p2pclient_connect(p2p, client()->mio, client()->dead_sess, client(), stream_SESS, NULL, client());
			
			if (connected){
				init_sess_st initpara;
				strcpy_s(initpara.from, sizeof(initpara.from) - 1, "monitor");
				strcpy_s(initpara.to, sizeof(initpara.to) - 1, dead->sname);
				p2pclient_init(p2p, &initpara);
			}
		}
		else if (dead->type == back_server_SESS){			
			sess_free(dead);
		}		
	}
}

//Ping协议
static void _pingsession(int waits)
{
	int i = 0;
	int count = 0;
	time_t now = time(0);
	static time_t last = 0;
	if(last == 0) last = now;

	if(now - last < waits) return ;

	last = now;

	count = jqueue_size(client()->session_que);

	for(;i<count;i++)
	{
		sess_t sess = (sess_t)jqueue_pull(client()->session_que);
		jqueue_push(client()->session_que, sess, 0);

		if(sess->binitsess == TRUE && sess->type == stream_SESS)
		{
			p2pclient_t  p2p  = (p2pclient_t)sess;			
			protobuffer_send(p2p,eProto_ping,NULL);
			log_write(client()->log, LOG_NOTICE, "ping %s.", sess->sname);
		}
	}
}

static int send2namend(char* name, char* buffer, int bufferlen)
{
	int rt = 0;
	if (name == NULL || buffer == NULL) return 0;

	sess_t sess = (sess_t)xhash_get(client()->sessions, name);
	if (sess)
	{
		response_pkt_p pkt;
		p2p_t p2p = (p2p_t)sess;

		buffer[bufferlen + 1] = '\0';

		pkt = response_pkt_new(client()->pktpool, bufferlen + 1);
		memcpy(pkt->data, buffer, bufferlen + 1);
		pkt->len = bufferlen + 1;

		p2p_stream_push(p2p, pkt);
		printf("send2named buffer = %s.\r\n", pkt->data);

		rt = 1;
	}

	return rt;
}

static char * offline_jsonstring(char userid[])
{
	char  * rslt = NULL;
	char    szvalue[64];
	char    sztime[64];
	cJSON * root = cJSON_CreateObject();

	SYSTEMTIME T;
	GetLocalTime(&T);
	sprintf(sztime, "%04d-%02d-%02d %02d:%02d:%02d", T.wYear, T.wMonth, T.wDay, T.wHour, T.wMinute, T.wSecond);



	cJSON_AddStringToObject(root, "type", "offline");

	cJSON_AddStringToObject(root, "userid", userid);

	cJSON_AddStringToObject(root, "time", sztime);

	/*snprintf(szvalue, sizeof(szvalue), "%d", g_instapp->seqno2smcache);
	cJSON_AddStringToObject(root, "seqno", szvalue);*/

	rslt = cJSON_PrintUnformatted(root);
	free(root);				// 避免内存泄漏

	return rslt;

}

static redisContext* redisinit()
{
	redisContext* c = redisConnect("127.0.0.1", 6379);
	if (c->err)
	{
		redisFree(c);
		printf("Connect to redisServer faile\n");
		return  NULL ;
	}
	return c;
	printf("Connect to redisServer Success\n");
}

int s_atoi(const char* p){
	if(p == NULL) 
		return 0;
	else 
		return atoi(p);
}

char* s_strdup(const char* src){
	if(src != NULL){
		int strl = strlen(src);
		char* p = (char*)malloc(strl+1);
		strcpy_s(p,strl+1,src);
		return p;
	}

	return NULL;
}

double s_atof(const char* str){
	if(str == NULL) return 0;
	return atof(str);
}

static int  mysqldb_que_ping(int waits)
{
	static int sqlcon_cnt = 0;
	static int code = 1;
	int i = 0;
	char * towncode;
	mysqlquery_t sub_query;
	static time_t last = 0;
	time_t now = time(0);
	if (last == 0)
		last = now;

	if (now - last < waits)
		return 1;

	last = now;

	sqlcon_cnt = jqueue_size(client()->sub_query_que);

	for (i = 0; i < sqlcon_cnt; i++)
	{
		mysqlquery_t query = (mysqlquery_t)jqueue_pull(client()->sub_query_que);	// 取出一个连接

		if (query && query->m_con)
		{
			code = mysql_ping(query->m_con);

			// 如果mysql_ping失败，则尝试重连该分数据库
			if (code)
			{
				towncode = (mysqlquery_t)xhash_get(client()->xsubquery2towncode, query);
				dbconfig_t dbconfig = (dbconfig_t)xhash_get(client()->xdbconfig, towncode);

				// 针对该towncode，查找对应分数据库连接属性，并尝试重建连接
				sub_query = mysqldb_connect_init(dbconfig->ip, atoi(dbconfig->port), dbconfig->username, dbconfig->password, dbconfig->dbname, NULL);
				if (sub_query == NULL)
				{
					log_write(client()->log, LOG_NOTICE, "sub_query[%d] reconnect to database [%s] failed. %d @%s\n", i, dbconfig->dbname, __LINE__, __FILE__);
					jqueue_push(client()->sub_query_que, query, 0);
					continue;
				}
				else
				{
					mysql_query(sub_query->m_con, "SET NAMES 'gb2312'");
					log_write(client()->log, LOG_NOTICE, "sub_query[%d] reconnected db [%s] ok.", i, dbconfig->dbname);

					xhash_zap(client()->xsubquery2towncode, query);	// 将原来的失效连接从哈希表中移除
					xhash_put(client()->xtowncode2subquery, towncode, sub_query);	// 以towncode为键，存储store_pid[i]线程与该分数据库的连接
					xhash_put(client()->xsubquery2towncode, sub_query, towncode); // 以store_pid[i]线程与某分数据库的连接为键，存储该分数据库对应的towncode值
					jqueue_push(client()->sub_query_que, sub_query, 0);				// 将该连接放入队列中，供store_pid[i]线程进行mysql_ping操作
				}
			}
			// 如果mysql_ping成功，则将连接放回队列
			else
			{
				jqueue_push(client()->sub_query_que, query, 0);
			}
		}

		log_write(client()->log, LOG_NOTICE, "mysql query[%d] code %d[%s]", i, code, code == 0 ? "success" : "failed");
	}

	return code;
}

/*注释*/
static void _app_config_expand(client_p client) 
{

	client->moniclient_ip = s_strdup("0.0.0.0");
	client->moniclient_port = j_atoi(config_get_one(client->config, "moniclient.port", 0), 0);
	if (client->moniclient_port == 0)
		client->moniclient_port = 3354;

	// frontsvr
	client->frontsvr_ip = s_strdup(config_get_one(client->config, "frontsvr.ip", 0));
	if (client->frontsvr_ip == NULL)
		client->frontsvr_ip = s_strdup("127.0.0.1");
	client->frontsvr_port = j_atoi(config_get_one(client->config, "frontsvr.port", 0), 0);
	if (client->frontsvr_port == 0)
		client->frontsvr_port = 7126;

	client->p2p_frontsvr = _strdup("");

	client->appattr.db_name = config_get_one(client->config, "db.name", 0); 
	client->appattr.db_ip = config_get_one(client->config, "db.ip", 0);
	client->appattr.db_port = j_atoi(config_get_one(client->config, "db.port", 0), 0);
	client->appattr.db_pwd = config_get_one(client->config, "db.pwd", 0);
	client->appattr.db_user = config_get_one(client->config, "db.user", 0);

	client->log_type = j_atoi(config_get_one(client->config, "log.type", 0), 0);
}

#define DEFAULT_PATH "./"
#define BD_USER_MAX		150
#define ALL_USER_MAX	5000

static void clear_zombie_from_lives();
static BOOL check_zombie(sess_t sess,time_t now);

static void   clear_deads_from_lives();
static void   check_dead_from_lives(sess_t sessdead);

static void   check_inactive_client(int sec);

static void   _mysql_querycallback(void* conn, int type, int code);
static void   _mysql_ping(void* conn, int waits);



static void test_xhash(xht ht)
{
	char * content;
	jqueue_t q;

	char * t1 = "1";
	jqueue_t q1 = jqueue_new();
	jqueue_push(q1, t1, 0);
	xhash_put(ht, t1, q1);

	char * t2 = "2";
	jqueue_t q2 = jqueue_new();
	jqueue_push(q2, t2, 0);
	xhash_put(ht, t2, q2);

	q = (jqueue_t)xhash_get(ht, "1");
	content = (char*)jqueue_pull(q);
	printf("q1:content = %s.\r\n",content);
	jqueue_push(q, t1, 0);

	q = (jqueue_t)xhash_get(ht, "2");
	content = (char*)jqueue_pull(q);
	printf("q2:content = %s.\r\n", content);
	jqueue_push(q, t2, 0);


	q = (jqueue_t)xhash_get(ht, "1");
	content = (char*)jqueue_pull(q);
	printf("q1:content = %s.\r\n", content);
	jqueue_push(q, t1, 0);

	q = (jqueue_t)xhash_get(ht, "2");
	content = (char*)jqueue_pull(q);
	printf("q2:content = %s.\r\n", content);
	jqueue_push(q, t2, 0);
}

static void test_xhash1(xht ht)
{
	char * content;
	jqueue_t q;

	q = (jqueue_t)xhash_get(ht, "1");
	content = (char*)jqueue_pull(q);
	printf("q1:content = %s.\r\n", content);

	q = (jqueue_t)xhash_get(ht, "2");
	content = (char*)jqueue_pull(q);
	printf("q2:content = %s.\r\n", content);

}

static void show_sub_taskids()
{
	int i;
	char * task;
	int subcnt = 0;

	if (client()->taskid)
	{
		subcnt = jqueue_size(client()->taskid);
		log_write(client()->log, LOG_NOTICE, "sub task cnt = %d.", subcnt);
		for (i = 0; i < subcnt; i++)
		{
			task = (char *)jqueue_pull(client()->taskid);
			jqueue_push(client()->taskid, task, 0);
			log_write(client()->log, LOG_NOTICE, "task[%d] = %s.", i, task);
		}
	}
}



void   check_inactive_client(int sec)
{
	jqueue_t q = users_act_check(client()->user_act, sec);
	if (q == NULL) return;

	int cnt = jqueue_size(q);

	for (int i = 0; i < cnt; i++){
		user_status_t s = jqueue_pull(q);
		
		if (s == NULL) continue;

		user_st user;
		//从数据库查询人员信息，包括其所属的村、镇、组；
		if (0 == db_loaduser(client()->sqlobj, s->key, &user)){
			char buffer[1024];
			char * jreqstr = offline_jsonstring(s->key);		// 拼好下线消息串备用

			int size = jqueue_size(client()->session_que);
			for (int i = 0; i < size; i++){
				sess_t sess = jqueue_pull(client()->session_que);
				jqueue_push(client()->session_que, sess, 0);

				if (((sess->usertype == _town_type) && strcmp(sess->towncode, user.towncode) == 0) || ((sess->usertype == _county_type)))
				{
					int nprint = 0;

					if (s->type == _user_bd){
						nprint += sprintf_s(buffer + nprint, sizeof(buffer) - nprint, "$bdmessage,smcache,%s,%s", sess->skey, jreqstr);
						nprint += sprintf_s(buffer + nprint, sizeof(buffer) - nprint, "*%02X\r\n", checksum(buffer, nprint));
					}
					else{
						nprint += sprintf_s(buffer + nprint, sizeof(buffer) - nprint, "$message,smcache,%s,%s", sess->skey, jreqstr);
						nprint += sprintf_s(buffer + nprint, sizeof(buffer) - nprint, "*%02X\r\n", checksum(buffer, nprint));
					}
					send2namend(sess->skey, buffer, nprint);
				}
			}
			free(jreqstr);		// 避免内存泄漏
			//user_free(&user);
		}
		else
		{
			log_write(client()->log, LOG_NOTICE, "check_inactive_client, error: db_loaduser failed.");
		}

		free(s);
	}

	jqueue_free(q);
}


void clear_zombies()
{
	int  i = 0;
	int  count = jqueue_size(client()->zombie_que);

	while (i++ < count)
	{
		sess_t sess = (sess_t)jqueue_pull(client()->zombie_que);

		if (sess == NULL) continue;

		if (sess->type == back_server_SESS)
		{
			log_write(client()->log, LOG_NOTICE, "%s @%d is a zombie, I will kill our sess.", sess->sname, sess->fd->fd);
			p2p_kill((p2p_t)sess);
		}
		else if (sess->type == stream_SESS)
		{
			log_write(client()->log, LOG_NOTICE, "No response from %s, I will kill myself here and now...", sess->sname);
			p2pclient_close((p2pclient_t)sess);
		}
	}
}

void remove_zombie_from_lives()
{	
	int  i = 0;
	int  count = jqueue_size(client()->session_que);
	time_t now = time(0);

	for(;i<count;i++)
	{
		sess_t sess = (sess_t)jqueue_pull(client()->session_que);

		if(sess == NULL) continue;

		if (check_zombie(sess, now))
		{
			if (sess->type == back_server_SESS)
			{
				jqueue_push(client()->zombie_que, sess, 0);
				/*log_write(client()->log, LOG_NOTICE, "%s @%d is a zombie, I will kill our sess.", sess->sname, sess->fd->fd);
				p2p_kill((p2p_t)sess);*/
			}
				
			else if (sess->type == stream_SESS)
			{
				jqueue_push(client()->zombie_que, sess, 0);
				/*log_write(client()->log, LOG_NOTICE, "No response from %s, I will kill myself here and now...",sess->sname);
				p2pclient_close((p2pclient_t)sess);*/
			}
				
		}
		else
			jqueue_push(client()->session_que,sess,0);
	}
}

void clear_deads_from_lives()
{
	int count = jqueue_size(client()->dead_sess);
	int i = 0;

	for(;i<count;i++){
		sess_t sess = (sess_t)jqueue_pull(client()->dead_sess);
		jqueue_push(client()->dead_sess,sess,0);
		check_dead_from_lives(sess);
	}
}

void check_dead_from_lives(sess_t sessdead)
{
	int i = 0;

	int count = jqueue_size(client()->session_que);

	for(;i < count ; i++){
		void* p = jqueue_pull(client()->session_que);
		if(sessdead == p) break;

		jqueue_push(client()->session_que,p,0);
	}

}

BOOL check_zombie(sess_t sess,time_t now){
	BOOL rtn = FALSE;

	if ((now - sess->last_activity) > 10){
		rtn = TRUE;
	}

	return rtn;
}

static void _mysql_querycallback(void* conn, int type)
{
	time_t now = time(NULL);

	if (type == eSqlQueryerr_errorquery || type == eSqlQueryerr_errorping){
		mysqlquery_t conn = NULL;

		mysqldb_close(client()->sqlobj);
		conn = mysqldb_connect_reinit(&client()->sqlobj, client()->appattr.db_ip, client()->appattr.db_port, client()->appattr.db_user, client()->appattr.db_pwd, client()->appattr.db_name);

		if (!mysqldb_isclosed(client()->sqlobj))
			log_write(client()->log, LOG_NOTICE, "connected db ok.");
		else
			log_write(client()->log, LOG_NOTICE, "connected db failed.");

	}
	else if (type == eSqlConnectOk)
	{
		mysqlquery_t connptr = (mysqlquery_t)conn;
		mysql_query(connptr->m_con, "SET NAMES 'gb2312'");

		log_write(client()->log, LOG_NOTICE, "connected db[%s] ok.", client()->appattr.db_name);
	}
}

static int init_database(_s_client_p client)
{
	mysqlquery_t sqlobj = NULL;
	sqlobj = mysqldb_connect_init(client->appattr.db_ip, client->appattr.db_port, client->appattr.db_user, client->appattr.db_pwd, client->appattr.db_name, _mysql_querycallback);
	if (sqlobj == NULL)
	{
		log_write(client->log, LOG_NOTICE, "connect to database failed. line%d @%s.", __LINE__, __FILE__);
		return -1;
	}
	client->sqlobj = sqlobj;
	
	return 0;
}

static void _mysql_ping(void* conn, int waits)
{
	int i = 0;
	int count = 0;
	time_t now = time(0);
	int pingcode;
	mysqlquery_t mysqlconn = (mysqlquery_t)conn;

	static time_t last = 0;

	if (last == 0) last = now;

	if (now - last < waits) return;
	last = now;

	pingcode = mysqldb_ping(mysqlconn);

	log_write(client()->log, LOG_NOTICE, "mysql query code %d[%s]", pingcode, pingcode == 0 ? "success" : "failed");
}

static int ping_maindb(mysqlquery_t query,int waits)
{
	static time_t last = 0;
	time_t now = time(0);
	if (last == 0)
		last = now;

	if (now - last < waits)
		return 1;

	last = now;

	int code = 1;

	if (query && query->m_con)
	{
		code = mysql_ping(query->m_con);
		log_write(client()->log, LOG_NOTICE, "mysql query code %d[%s]", code, code == 0 ? "success" : "failed");
	}
		
	if (code == 1 && query && query->_cb){
		log_write(client()->log, LOG_NOTICE, "Trying to reconnect database...");
		query->_cb(query, eSqlQueryerr_errorping);
	}

	return code;
}

int main(int argc, char **argv)
{
	int i = 0;
	p2pclient_t  p2p = 0;
	int   counter = 0;
	char*           config_file = NULL;
	time_t          lastcheckperson = time(0);
	jqueue_t        inactivequeue = jqueue_new();
#ifdef HAVE_WINSOCK2_H
	_MAC_WIN_WSA_START(0);
#endif

	jabber_signal(SIGTERM, _sensor_signal);

#ifndef _WIN32
	jabber_signal(SIGPIPE, SIG_IGN);
#endif
	set_debug_flag(0);

	client()->config = config_new();
	config_file = "../../etc/monitor.xml";
	if (config_load(client()->config, config_file) != 0)
	{
		fputs("sensor: couldn't load config, aborting\n", stderr);
		client_free(client());
		return 2;
	}

	_app_config_expand(client());

	if (client()->log_type == log_STDOUT)
		client()->log = log_new(log_STDOUT, NULL, NULL);
	else if (client()->log_type == log_FILE)
		client()->log = log_new(log_FILE, "../log/monitor.log", NULL);

	client()->pktpool = response_pkt_pool_new();
	//client()->user_act = users_act_new(8192);

	init_database(client()); //连接数据库

	client()->stream_maxq = 1024;
	client()->sessions = xhash_new(1023);
	//client()->xsubbytaskid = xhash_new(1023);
	client()->session_que = jqueue_new();
	client()->zombie_que = jqueue_new();
	client()->dead_sess = jqueue_new();
	client()->taskid = jqueue_new();
	//client()->subjects = subject_new(512);

	client()->mio = mio_new(FD_SETSIZE);
	client()->enable_p2p = TRUE;

	g_xsubbytaskid = xhash_new(1023);
	g_xtaskidexecutor = xhash_new(1023);

	p2p_listener_start(client());

	//连接frontsvr
	p2p = p2pclient_new(client()->frontsvr_ip, client()->frontsvr_port, stream_SESS); //tcp连接
	if (!p2pclient_connect(p2p, client()->mio, client()->dead_sess, client(), stream_SESS, NULL, client())){
		printf("connect to  frontsvr failed ,exit..\n");
		client_free(client());
		return 1;
	}
	else{
		p2p->expat = frontsvr_exp_new(&p2p->sess); //回调函数
		exp_set_callback(p2p->expat, frontsvr_exp_cb);
		init_sess_st initpara;
		strcpy_s(initpara.from, sizeof(initpara.from) - 1, "monitor");
		strcpy_s(initpara.to, sizeof(initpara.to) - 1, "frontsvr");
		p2pclient_init(p2p, &initpara);
	}
	client()->p2p_frontsvr = p2p;

	while (!g_shutdown)
	{
		static time_t last = 0;
		time_t now = time(NULL);
		if (last == 0) last = now;

		mio_run(client()->mio, 1);

		clear_deads_from_lives();

		_save_deadsession(7);

		if (now - last > 2){
			show_sub_taskids();
			remove_zombie_from_lives();
			clear_zombies();
			last = now;
		}
		_pingsession(2);
		_mysql_ping(client()->sqlobj, 60);

	}

	client_free(client());


	return 0;
}


