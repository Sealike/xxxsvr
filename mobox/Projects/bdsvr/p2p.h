#ifndef __P2P_H__
#define __P2P_H__
#include "instapp.h"
#include "responsepkt.h"
#include "buffer.h"
#include "expat.h"
#ifdef __cplusplus
extern "C"
{
#endif
struct _p2p_st {
	sess_st      sess;

	char                *p2pkey;
	int					use_udp_channel;

	char 				*ip;
	int					port;	

	jqueue_t			rcmdq;
	sx_buf_t			buf;

	jqueue_t 			wbufq;
	response_pkt_p	    wbufpending;

	int					want_read;
	int					want_write;

	_exp_t 		        expat;

	int                 init_sream_header;
	int                 init_audio_header;
	int					want_result_header;
	int                 max_queue;	
};
typedef struct _p2p_st p2p_st, *p2p_t;

int  p2p_stream_push(p2p_t p2p, response_pkt_p packet);
void p2p_listener_start(instapp_t client);
void p2p_kill(p2p_t p2p);
#ifdef __cplusplus
}
#endif
#endif