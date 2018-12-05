#ifndef __RESPONSE_PKT__
#define __RESPONSE_PKT__
#include "util/util.h"

typedef struct _response_pkt_pool_st{
	jqueue_t cacheque_use;
	jqueue_t cacheque_free;
	pthread_mutex_t mt;
}response_pkt_pool_st,*response_pkt_pool_t;

typedef struct _s_response_pkt{
	char * data;
	unsigned int len;

	void * heap;
	unsigned int size;
	response_pkt_pool_t pool;
}response_pkt;
typedef response_pkt* response_pkt_p;

response_pkt_pool_t response_pkt_pool_new();
void           response_pkt_pool_free();
response_pkt_p response_pkt_new(response_pkt_pool_t pool,int len);
void           response_pkt_free(response_pkt_p pkt);
response_pkt_p response_pkt_clone(response_pkt_p pkt);

#endif