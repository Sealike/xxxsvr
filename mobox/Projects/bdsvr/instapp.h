#ifndef __INSTAPP_H__
#define __INSTAPP_H__

#include "mio/mio.h"
#include "util/xhash.h"
#include "util/util.h"
#include "responsepkt.h"
#include <time.h>
#include "jobqueue.h"


#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _serverattr_st{
	char  *db_driver;
	char  *server_ip;
	int	   server_port;
	mio_fd_t server_fd;
	char* db_ip;
	int   db_port;
	char* db_name;
	char* db_user;
	char* db_pwd;
	char* stgip;
	int   stgport;

}serverattr_st, serverattr_t;


typedef struct _instapp_st
{
	BOOL                	enable_p2p;
	mio_t               	mio;

	int						bdjob_num;

// 数据库相关
	serverattr_st       appattr;		
	void*               sqlobj;

// frontend相关	
	char		    	*bdjob_ip;	// TCP监听
	int					bdjob_port;
	mio_fd_t			bdjob_fd;


	//北斗任务相关
	jqueue_t             jobqueue;
	// frontsvr相关
	void               	*p2p_frontsvr;
	char               	*frontsvr_ip;  //frontsvr服务器的信息
	int                 frontsvr_port;


	// seqno相关
	int					seqno2frontsvr;

	// http client相关
	void               	*httpc;
	char               	*https_ip; //http 服务器的信息
	int                  https_port;
	char				*httpc_cmd;



	// auth相关
	int					auth_userid;	// 指挥机本身的北斗号
	char				*auth_pwd;
	char				*auth_name;		// 服务连接的名字：bdsvr01
	char				*auth_role;		// 服务连接的角色：bdsvr

	xht                 	sessions;
	xht                 	namedend;
	jqueue_t            	session_que;
	jqueue_t                zombie_que;
	jqueue_t            	dead_sess;
	
	int                 	stream_maxq;
	config_t            	config;
	response_pkt_pool_t 	pktpool;

	/** logging */
	log_t              	log;

	/** log data */
	log_type_t         	log_type;
	char              	*log_facility;
	char              	*log_ident;
}instapp_st, *instapp_t;


instapp_t instapp_new();
void      instapp_free(instapp_t instapp);


typedef enum {
	stream_SESS  		= 0x00,
	mon_SESS     		= 0x01,
	bdjobber_SESS       = 0x03,
	front_SESS     		= 0x04,	
	p2pclient_SESS 		= 0x06,
	storage_SESS 		= 0x07,
	open_SESS			= 0x08,
	bdjob_SESS			= 0x09
}sess_type_t;

typedef enum {
	state_init_SESS 	= 0x00,
	state_handshake_SESS  	= 0x01,
	state_open_SESS 	= 0x02
}sess_state_t;

typedef struct _sess_st {
	sess_type_t  	type;
	char		skey[64];
	char         	sname[64];
	time_t       	last_activity;
	instapp_t    	instapp;
	mio_fd_t	fd;
	int          	state;
	BOOL         	binitsess;//是否已经Initsess了
} sess_st, *sess_t;

extern instapp_t g_instapp ;
#ifdef __cplusplus
}
#endif
#endif