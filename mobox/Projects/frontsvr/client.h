#ifndef __CLIENT_H__
#define __CLIENT_H__
#include "mio/mio.h"
#include "util/xhash.h"
#include "util/util.h"
#include "responsepkt.h"
#include "subject.h"
#include "userstatus.h"
#include "rediscommand.h"
#include "dbaccess.h"

#include <time.h>

//typedef struct _conf_st
//{
//	int warntime;			// 消失报警的阈值时间，单位是分钟
//}conf_st, *conf_t;

typedef enum {
	stream_SESS  = 0x00,
	mon_SESS     = 0x01,
	back_server_SESS   = 0x03,
	front_mcu_SESS     = 0x04,	
	p2pclient_SESS = 0x06,
	storage_SESS = 0x07
}sess_type_t;

typedef enum {
	state_init_SESS = 0x00,
	state_handshake_SESS  = 0x01,
	state_open_SESS = 0x02
}sess_state_t;

#define  WARN_NONE       (0)
#define  WARN_AREA_ILL   (1<<0)
#define  WARN_AGRE       (1<<1)
#define  WARN_GROUP_SEPT (1<<2)
#define  WARN_STRANGER   (1<<3)
#define  WARN_AREA_NONE  (1<<4)
#define  WARN_DISAPPEAR  (1<<5)
#define  WARN_CACHEAREA  (1<<6)
typedef struct _mytime_st 
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
}mytime_st,*mytime_t;

typedef struct _weektime_st{
	unsigned char weekmask;

	int hour;
	int minute;
	int second;
}weektime_st,*weektime_t;

typedef struct _weektimesection_st 
{
	unsigned char weekmask;

	int hourStart;
	int minuteStart;
	int secondStart;

	int hourEnd;
	int minuteEnd;
	int secondEnd;
}weektimesection_st,*weektimesection_t;

void weektimesection_2str(weektimesection_t t,char* str);
int  weektime_in(weektime_t t,weektimesection_t s);
int  mytime_cmp_sub(mytime_t l,mytime_t r);


typedef struct _fault_st{
	char ip[16];
	int  fcode;//error code
}fault_st,*fault_t;

typedef enum{
	ePerson = 0,
	eArea   = 1,
	ePlan   = 2,
	eRule   = 3,
	eGroup  = 4,
	eFixrule= 5,
	eFormation =6,
	eDBCTag = 7,
	eDBCUnused = 100,
}eDBC_obj;

typedef  enum{
	eADD = 0,
	eDEL = 1,
	eUPD = 2,
}eDBC_operation;


typedef struct _dbc_person_st
{
	eDBC_operation type;
	int  pid;//person id.
	int  gid;//group belong to.
	int  oldgid;//obsolet group belonged to.
}dbc_person_st,*dbc_person_t;

typedef struct _dbc_area_st
{
	eDBC_operation type;
	char areaid[64];
	int  x;
	int  y;
	int  z;
	int  cx;
	int  cy;
	int  alertable;
	int  alertlevel;
}dbc_area_st,*dbc_area_t;

typedef struct _dbc_plan_st
{
	eDBC_operation type;
	int  planid;
}dbc_plan_st,*dbc_plan_t;

typedef struct _dbc_rule_st
{
	eDBC_operation type;
	int  personid;
	char areacode[64];
	mytime_st start;
	mytime_st end;
	int  planid;
	int  grouprule;
}dbc_rule_st,*dbc_rule_t;

typedef struct _dbc_fixrule_st{
	eDBC_operation type;
	int   personid;
	char areacode[64];
	weektimesection_st section;
	int   planid;
	int   grouprule;
}dbc_fixrule_st,*dbc_fixrule_t;

typedef struct _dbc_formation_st{
	eDBC_operation type;
	int   personid;
	int   groupid;
}dbc_formation_st,*dbc_formation_t;

typedef struct _dbc_group_st
{
	eDBC_operation type;
	int groupid;
	int leaderid;
	int tempflag;
	mytime_st starttime;
	mytime_st endtime;
}dbc_group_st,*dbc_group_t;

typedef struct _dbc_info_st{
	eDBC_obj otype;
	int      dbctag;
	union{
		dbc_person_st person;
		dbc_area_st   area;
		dbc_plan_st   plan;
		dbc_rule_st   rule;
		dbc_group_st  group;
		dbc_fixrule_st fixrule;
		dbc_formation_st formation;		
	};
}dbc_info_st,*dbc_info_t;

#define  USER_INFO_ID    (1001)
typedef struct _vec_st{
	int    cnt;
	int    cap;
	int    size;
	char*  data;

}*vec_t,vec_st;

vec_t   vec_alloc(vec_t,int cnt,int size);
int     vec_add  (vec_t,int id ,int x,int y,int z);
void    vec_clear(vec_t);
void    vec_free (vec_t);
void*   vec_data (vec_t,int);

typedef struct _warn_st{
	int   personid;
	BOOL  redlight;
	BOOL  greenlight;
	BOOL  vibration;
	BOOL  beep;
}warn_st,*warn_t;

#define  WARN_INFO_ID    (1002)

typedef struct _resp_header_st{
	int type;
	int len;
}resp_header_st,*resp_header_t;

typedef struct _serverattr_st{
	char* db_driver;
	char* db_name;
	char* db_ip;
	int   db_port;
	char* db_user;
	char* db_pwd;
	char* infosvr_ip;
	int   infosvr_port;
	int   listen_pos_port;
	float allow_group_distance;//m
	float allow_pos_deviation;
	int   allow_pos_timeout;//s
	int   persons_cache_size;
	int   rules_cache_size;
	int   areas_cache_size;
	int   groups_cache_size;
	char* stgip;
	int   stgport;

}serverattr_st,serverattr_t;

struct _s_client{
	mio_t               mio;
	serverattr_st       appattr;

	// client 
	char			   *frontend_ip;
	int			        frontend_port;
	mio_fd_t			frontend_fd;

	char			   *backend_ip;
	int			        backend_port;
	mio_fd_t			backend_fd;


	//server port..
	char			   *warnsvr_ip;
	int			        warnsvr_port;
	mio_fd_t			warnsvr_fd;
	void               *p2p_warnsvr;


	
	void               *devmap;
	subject_t          subjects;
	//
	users_act_t        user_act;
	//
	void               *mysqlconn;
	int                 monitor_id;
	int                 tracker_id;
	//conf_t				plat_conf;

	xht                 sessions;
	jqueue_t            session_que;
	jqueue_t            zombie_que;
	jqueue_t            dead_sess;
	BOOL                enable_p2p;
	int                 stream_maxq;
	config_t            config;
	response_pkt_pool_t pktpool;	
	char                response_buff[40960];
	/** logging */
	log_t              log;
	/** log data */
	log_type_t         log_type;
	char              *log_facility;
	char              *log_ident;
	void*               sqlobj;

	char				*aes_pwd;

	int					redis_clife;
	int					redis_blife;
	redisContext		*redisUserinfo;

	xht					xtowncode2subquery;
	xht					xsubquery2towncode;
	xht					xdbconfig;
	jqueue_t			sub_query_que;
};
typedef struct _s_client  *_s_client_p, *client_p;

typedef struct _sess_st {
	sess_type_t  type;
	char		 skey[64];
	char         sname[64];
	char         role[64];
	char         subject[64];
	time_t       last_activity;
	_s_client_p  client;
	mio_fd_t	 fd;
	int          state;
	void*        param;  
	BOOL         binitsess;//是否已经Initsess了
	BOOL		 auth;
	int			 auth_cnt;	// 登录验证的次数
	char         towncode[64];
	int          usertype;
} sess_st, *sess_t;

typedef struct _mhash_cache_st{
	xht    t;
}mhash_cache_st,*mhash_cache_t;

void mhash_cache_free(mhash_cache_t cache);
void mhash_cache_clear(mhash_cache_t cache);

typedef int (*pftmio_callback)(mio_t m, mio_action_t a, mio_fd_t fd, void *data, void *arg);
void sess_free(sess_t sess);
void client_free(client_p c);
void client_push_pkt(client_p c,response_pkt_p pkt);

void mytime2str(mytime_t t,char* str);
void str2mytime(char* str,mytime_t t);
weektime_t myweektime(weektime_t wt);
mytime_t  mylocaltime(mytime_t t);
mytime_t localtime2mytime(struct tm* src,mytime_t dest);
int str2hms(char* str,int* hour,int* minute,int* second);

#define _DAYSECONDS(hour,minute,second) (hour*3600+minute*60+second)
#define _SOW(day,hour,minute,second) (day*86400+hour*3600 + minute * 60 + second)

_s_client_p client();
_s_client_p getclient();

#define _county_type (0)
#define _town_type   (1)

#endif