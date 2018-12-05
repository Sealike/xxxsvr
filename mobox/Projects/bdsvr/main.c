// main.c : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "instapp.h"
#include "frontsvr_expat.h"
#include "bdjobber_callback.h"
#include "frontsvr_callback.h"
#include "protobuffer.h"
#include "dbaccess.h"
#include "jobqueue.h"
#include "httpclient.h"
#include "myquery/mysqldb.h"
#include "client.h"

#define BDJOB_PORT 	3348
#define FRONTSVR_PORT	7120
#define SEQNO2FRONTSVR  102
#define BD2FRONTSVR_SEQNO_MAX 65536		// 远小于unsigned int 型数据的最大值

static void _mysql_querycallback(void* conn, int type);


// 全局变量定义
instapp_t g_instapp = NULL;
int g_shutdown = 0;

#ifdef __cplusplus
extern "C"
{
#endif
BOOL jobber_free = FALSE;
#ifdef __cplusplus
}
#endif

static BOOL check_zombie(sess_t sess, time_t now);

static void remove_zombie_from_lives()
{	
	int  i = 0;
	int  count = jqueue_size(g_instapp->session_que);
	time_t now = time(0);

	for(;i<count;i++)
	{
		sess_t sess = (sess_t)jqueue_pull(g_instapp->session_que);

		if(sess == NULL) continue;

		if (check_zombie(sess, now))
		{
			if (sess->type == bdjobber_SESS)
				jqueue_push(g_instapp->zombie_que, sess, 0);// p2p_kill((p2p_t)sess);
			else if (sess->type == stream_SESS)
			{
				log_write(g_instapp->log, LOG_NOTICE, "%s does not act for 7s, so I closed our sess.", sess->sname);
				//p2pclient_close((p2pclient_t)sess);
				jqueue_push(g_instapp->zombie_que, sess, 0);
			}
				
		}	
		else
			jqueue_push(g_instapp->session_que,sess,0);
	}
}

static void clear_zombies()
{
	int  i = 0;
	int  count = jqueue_size(g_instapp->zombie_que);

	for (; i < count; i++)
	{
		sess_t sess = (sess_t)jqueue_pull(g_instapp->zombie_que);

		if (sess == NULL) continue;

		if (sess->type == bdjobber_SESS)
			p2p_kill((p2p_t)sess);
		else if (sess->type == stream_SESS)
		{
			log_write(g_instapp->log, LOG_NOTICE, "%s does not act for 7s, so I closed our sess.", sess->sname);
			p2pclient_close((p2pclient_t)sess);
		}
	}
}

static void check_dead_from_lives(sess_t sessdead)
{
	int i = 0;
	int count = jqueue_size(g_instapp->session_que);

	for(;i < count ; i++){
		void* p = jqueue_pull(g_instapp->session_que);
		if(sessdead == p) break;

		jqueue_push(g_instapp->session_que,p,0);
	}
}
static void clear_deads_from_lives()
{
	int count = jqueue_size(g_instapp->dead_sess);
	int i = 0;

	for(;i<count;i++){
		sess_t sess = (sess_t)jqueue_pull(g_instapp->dead_sess);
		jqueue_push(g_instapp->dead_sess,sess,0);
		check_dead_from_lives(sess);
	}
}
static void _save_deadsession(int waits)
{
	int count = 0;
	int i = 0;
	time_t now = time(0);
	static time_t last = 0;

	if(last == 0) last = now;

	if(now - last < waits) return ;

	last = now;

	count = jqueue_size(g_instapp->dead_sess);

	for(;i < count ;i++)
	{
		sess_t dead = (sess_t)jqueue_pull(g_instapp->dead_sess);		
		if (dead->type == stream_SESS){
		{
			p2pclient_t p2p = (p2pclient_t)dead;
			BOOL connected = p2pclient_connect(p2p, g_instapp->mio, g_instapp->dead_sess, g_instapp, stream_SESS, NULL, g_instapp);
		}	
		}
		else if (dead->type == bdjobber_SESS)
		{
			sess_free(dead);//不再保留记录，释放会话。
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

	count = jqueue_size(g_instapp->session_que);

	for(;i<count;i++)
	{
		sess_t sess = (sess_t)jqueue_pull(g_instapp->session_que);
		jqueue_push(g_instapp->session_que, sess, 0);
		if(sess->binitsess == TRUE && sess->type == stream_SESS)
		{
			p2pclient_t  p2p  = (p2pclient_t)sess;			
			protobuffer_clnt_send(p2p,eProto_ping,NULL);
			log_write(g_instapp->log, LOG_NOTICE, "ping %s.", sess->sname);
		}
	}
}

static BOOL check_zombie(sess_t sess,time_t now){
	BOOL rtn = FALSE;

	if((now-sess->last_activity) > 7){		
		rtn = TRUE;
	}

	return rtn;
}

/*注释*/
static void _app_config_expand(instapp_t instapp) 
{	
	char * strip = NULL;

	instapp->bdjob_num = 0;

	/* seqno2frontsvr */
	instapp->seqno2frontsvr = j_atoi(config_get_one(instapp->config, "seqno.tofrontsvr", 0), 0);
	if (instapp->seqno2frontsvr == 0) instapp->seqno2frontsvr = SEQNO2FRONTSVR;

	/* bdjob */
	instapp->bdjob_ip  = _strdup("0.0.0.0");
	instapp->bdjob_port = j_atoi(config_get_one(instapp->config, "bdjob.port", 0),0);
	if(instapp->bdjob_port == 0) instapp->bdjob_port = BDJOB_PORT;	

	/* frontsvr */
	strip  = config_get_one(instapp->config, "frontsvr.ip", 0);
	if(strip == NULL)
		instapp->frontsvr_ip = _strdup("127.0.0.1");
	else
		instapp->frontsvr_ip = _strdup(strip);
	instapp->frontsvr_port = j_atoi(config_get_one(instapp->config, "frontsvr.port", 0), 0);
	if (instapp->frontsvr_port == 0)
		instapp->frontsvr_port = FRONTSVR_PORT;
	instapp->p2p_frontsvr = _strdup("");


	/* db */
	instapp->appattr.db_name = config_get_one(instapp->config, "db.dbname", 0);
	instapp->appattr.db_ip = config_get_one(instapp->config, "db.ip", 0);	
	instapp->appattr.db_port = j_atoi(config_get_one(instapp->config, "db.port", 0), 0);
	instapp->appattr.db_user = config_get_one(instapp->config, "db.user", 0);;
	instapp->appattr.db_pwd = config_get_one(instapp->config, "db.pwd", 0);;

	/* auth */
	instapp->auth_userid = j_atoi(config_get_one(instapp->config, "auth.userid", 0),0);
	instapp->auth_pwd = config_get_one(instapp->config, "auth.pwd", 0);
	instapp->auth_name = config_get_one(instapp->config, "auth.name", 0);
	instapp->auth_role = config_get_one(instapp->config, "auth.role", 0);
}

static int init_database(instapp_t client)
{
	mysqlquery_t sqlobj = NULL;
	sqlobj = mysqldb_connect_init(client->appattr.db_ip, client->appattr.db_port, client->appattr.db_user, client->appattr.db_pwd, client->appattr.db_name, _mysql_querycallback);
	if (sqlobj == NULL)
	{
		log_write(client->log, LOG_NOTICE, "connect to database failed. server exit. line%d @%s\n", __LINE__, __FILE__);
		g_shutdown = TRUE;
	}

	client->sqlobj = sqlobj;

	return client->sqlobj != 0;
}

static void _mysql_querycallback(void* conn, int type)
{
	time_t now = time(NULL);

	if (type == eSqlQueryerr_errorquery || type == eSqlQueryerr_errorping){
		mysqlquery_t conn = NULL;

		mysqldb_close(g_instapp->sqlobj);
		conn = mysqldb_connect_reinit(&g_instapp->sqlobj, g_instapp->appattr.db_ip, g_instapp->appattr.db_port, g_instapp->appattr.db_user, g_instapp->appattr.db_pwd, g_instapp->appattr.db_name);

		if (!mysqldb_isclosed(g_instapp->sqlobj))
			log_write(g_instapp->log, LOG_NOTICE, "connected db ok\n");
		else
			log_write(g_instapp->log, LOG_NOTICE, "connected db failed\n");

	}
	else if (type == eSqlConnectOk)
	{
		mysqlquery_t connptr = (mysqlquery_t)conn;
		mysql_query(connptr->m_con, "SET NAMES 'gb2312'");

		log_write(g_instapp->log, LOG_NOTICE, "connected db [%s] ok\n", g_instapp->appattr.db_name);
	}
}

static unsigned char checksum(unsigned char buffer[], int len)
{
	unsigned char data = 0;
	int i = 0;
	for (; i < len; i++)
		data ^= buffer[i];

	return data;
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

	log_write(g_instapp->log, LOG_NOTICE, "mysql query code %d[%s]", pingcode, pingcode == 0 ? "success" : "failed");
}

int _tmain(int argc, _TCHAR* argv[])
{
	//const char * test = "$BDBSI,312194([3,4,4,4,4,4,0,0,0,0])A4";
	char ctemp[6] = { 0 };
	p2pclient_t  p2p_frontsvr = 0;
	p2pclient_t  p2p_auth = 0;

	const char* config_file = "../../etc/bdsvr.xml";

	//unsigned char *buffer = "CCTXA,0455943,1,2,A44246303131334543434248656C6C6F";
	//unsigned char *buffer = "CCTXA,0455943,1,2,A4BF0113ECCB13ECCBHELLO";
	//unsigned char *buffer = "CCTXA,0455943,1,2,A4BF01012C8BAB7A2C4DAC8DD";
	//unsigned char * buffer = "BDTXR,1,195817,2,1439,A4BF0113ECCB13ECCBHELLO";
	//unsigned char * buffer = "BDTXR,1,195817,2,1439,A4BF0113ECCB13ECCB48656C6C6F";
	/*unsigned char * buffer = "BDWAA,0,,0312194,072236.40,3802.69000,N,11526.16000,E,61,M";
	printf("checksum = %d\r\n", checksum(buffer, strlen(buffer)));
	while (1);*/

	g_instapp = instapp_new();

	g_instapp->log 		= log_new(log_STDOUT,NULL,NULL);
	g_instapp->pktpool 	= response_pkt_pool_new();
	g_instapp->config  	= config_new();
	//g_instapp->jobqueue = jobqueue_new();
	g_instapp->jobqueue = jqueue_new();

	if(config_load(g_instapp->config, config_file) != 0)
	{
		fputs("bdsvr: couldn't load config, aborting\n", stderr);
		instapp_free(g_instapp);
		return 2;
	}

	_app_config_expand(g_instapp);
	init_database(g_instapp);

	log_debug(ZONE, "seqno = %d.\r\n", g_instapp->seqno2frontsvr);

	g_instapp->stream_maxq 	= 1024;	
	g_instapp->sessions    	= xhash_new(1023);
	g_instapp->session_que 	= jqueue_new();
	g_instapp->dead_sess   	= jqueue_new();
	g_instapp->zombie_que   = jqueue_new();
	g_instapp->mio 			= mio_new(FD_SETSIZE);	
	g_instapp->enable_p2p 	= TRUE;

	p2p_listener_start(g_instapp);		// TCP监听窗口，负责监听bdjobber端口

	//explain_bsi_my(test, int nFrameLen, const char *DataBuff, DWORD &Len)
	
	//frontsvr服务器
	p2p_frontsvr = p2pclient_new(g_instapp->frontsvr_ip, g_instapp->frontsvr_port, stream_SESS); //tcp连接
	if (!p2pclient_connect(p2p_frontsvr, g_instapp->mio, g_instapp->dead_sess, g_instapp, stream_SESS, NULL, g_instapp))
	{
		log_debug(ZONE, "connect to  frontsvr failed ,exit..");
		instapp_free(g_instapp);
		return 1;
	}
	else
	{
		p2p_frontsvr->expat = frontsvr_exp_new(&p2p_frontsvr->sess);	// 解析器
		exp_set_callback(p2p_frontsvr->expat, frontsvr_exp_cb);		// 解析器的回调函数
	}
	g_instapp->p2p_frontsvr = p2p_frontsvr;

	while(!g_shutdown)
	{
		static time_t last = 0;
		time_t now = time(NULL);

		if(last == 0) last = now;

		mio_run(g_instapp->mio,1);

		clear_deads_from_lives();	

		_save_deadsession(7);

		if(now - last > 2){
			remove_zombie_from_lives();
			last = now;
			clear_zombies();
		}

		if ((jobber_free == TRUE) && jqueue_size(g_instapp->jobqueue) > 0)
		{

			if (0 != ParseEncodeSend())
			{
				jobber_free = FALSE;
				log_write(g_instapp->log, LOG_NOTICE, "set jobber_free to [FALSE] in main.");
			}	
		}

		_pingsession(2);
		_mysql_ping(g_instapp->sqlobj, 60);
		
	}

	instapp_free(g_instapp);

	return 0;
}

