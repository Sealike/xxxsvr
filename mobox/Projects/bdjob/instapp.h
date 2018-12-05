#ifndef __INSTAPP_H__
#define __INSTAPP_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "mio/mio.h"
#include "util/xhash.h"
#include "util/util.h"
#include "responsepkt.h"
#include <time.h>

typedef struct _deviceinfo_st
{
	int   icomport;
	int   iprotocal; //Э�� 0��4.0  1��2.1
	int   ibaud;
}deviceinfo_st, *deviceinfo_t;
typedef struct _instapp_st
{
	mio_t               mio;	
	int			        fe_port;

	char               *bdsvr_ip;  //bdsvr����������Ϣ
	int                 bdsvr_port;
	void               *p2pbdsvr;
	xht                 sessions;
	xht                 namedend;
	jqueue_t            session_que;
	jqueue_t            zombie_que;
	jqueue_t            dead_sess;
	mio_fd_t			fe_fd;	
	int                 stream_maxq;
	config_t            config;
	response_pkt_pool_t pktpool;
	/** logging */
	log_t              log;
	/** log data */
	log_type_t         log_type;
	char              *log_facility;
	char              *log_ident;
	int   idecvicecount;//�����ն˸���
	int   ihascache;//�豸�����Ƿ��л��湦��
	deviceinfo_st deviceinfo[10];
}instapp_st, *instapp_t;


instapp_t instapp_new();
void      instapp_free(instapp_t instapp);


typedef enum {
	stream_SESS  = 0x00,
	mon_SESS     = 0x01,
	back_server_SESS    = 0x03,
	front_SESS     = 0x04,	
	p2pclient_SESS = 0x06,
	storage_SESS = 0x07,
}sess_type_t;

typedef enum {
	state_init_SESS = 0x00,
	state_handshake_SESS  = 0x01,
	state_open_SESS = 0x02
}sess_state_t;

typedef struct _sess_st {
	sess_type_t  type;
	char		 skey[64];
	char         sname[64];
	time_t       last_activity;
	instapp_t    instapp;
	mio_fd_t	 fd;
	int          state;
	BOOL         binitsess;//�Ƿ��Ѿ�Initsess��
} sess_st, *sess_t;

#ifdef __cplusplus
}
#endif
#endif