#ifndef  _P2P_CLIENT_H__
#define  _P2P_CLIENT_H__
#include "responsepkt.h"
#include "buffer.h"
#include "instapp.h"
#include "util/util.h"
#include "expat.h"
#ifdef __cplusplus
extern "C"
{
#endif
typedef void (*io_complete_cb)(void *data, int len, void *arg);

struct _p2pclient_st {
	sess_st				sess;
	char 				*ip;
	int					port;	

	jqueue_t			rcmdq;
	sx_buf_t			buf;

	jqueue_t 			wbufq;
	response_pkt_p	    wbufpending;

	response_pkt_pool_t pktpool;
	int					want_read;
	int					want_write;
	jqueue_t            deadsess;
	int                 max_queue;
	mio_t               mio;
	BOOL                lost;
	void*               client;
	io_complete_cb      iccb;
	_exp_t              expat;
	void*               para;
	
};

typedef struct _p2pclient_st p2pclient_st, *p2pclient_t;

p2pclient_t p2pclient_new(char* strip,int port,sess_type_t sesstype);
void p2pclient_free(p2pclient_t p);
void p2pclient_setup(p2pclient_t p,sess_type_t sesstype);
int  p2pclient_write(p2pclient_t p2p, response_pkt_p packet);
BOOL p2pclient_connect(p2pclient_t p,mio_t mio,jqueue_t deadsess,void* param,sess_type_t sesstype,io_complete_cb iccb,void* para);
void p2pclient_close(p2pclient_t p2p);

#ifdef __cplusplus
}
#endif

#endif