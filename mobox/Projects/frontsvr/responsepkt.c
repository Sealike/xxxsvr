#include "responsepkt.h"
#include "util/util.h"
#include "util/pool.h"

#define  NULL  (0)
#define  RESONSE_MTU  (64*1024)

response_pkt_pool_t response_pkt_pool_new()
{
	response_pkt_pool_t rtn = NULL;
	
	while((rtn =calloc(1,sizeof(response_pkt_pool_st))) == NULL) Sleep(1) ;
	
	rtn->cacheque_use  = jqueue_new();
	rtn->cacheque_free = jqueue_new();
	pthread_mutex_init(&rtn->mt, NULL);

	return rtn;
}

void response_pkt_pool_free()
{

}

response_pkt_p response_pkt_new(response_pkt_pool_t pool,int size)
{
	response_pkt_p packet = NULL;

	if(size > RESONSE_MTU || pool == NULL) return NULL;

	pthread_mutex_lock(&pool->mt);

	if(jqueue_size(pool->cacheque_free) == 0)
	{
		packet = calloc(1,sizeof(response_pkt));
		if(packet!=NULL){
			packet->size = size;
			packet->heap = packet->data = calloc(1,size);
			packet->pool = pool;
			jqueue_push(pool->cacheque_use,packet,0);
		}
	}else{
		int i = 0;
		int count = jqueue_size(pool->cacheque_free);
		response_pkt* p = NULL;
		for(; i < count ; i++){
			p = jqueue_pull(pool->cacheque_free);
			if(p->size >= size)
			{
				jqueue_push(pool->cacheque_use,p,0);
				packet = p;
				break;
			}else{
				jqueue_push(pool->cacheque_free,p,0);
			}
		}

		if(packet==NULL){
			packet = calloc(1,sizeof(response_pkt));
			if(packet!=NULL){  
				packet->size = size;
				packet->heap = packet->data = calloc(1,size);
				packet->pool = pool;
				jqueue_push(pool->cacheque_use,packet,0);
			}
		}
	}

	pthread_mutex_unlock(&pool->mt);

	assert(packet->len == 0 && packet->data == packet->heap);

	return(packet);
}

void  response_pkt_free(response_pkt_p packet)
{
	response_pkt* p = NULL;
	int count ;
	int i = 0 ;
	response_pkt_pool_t pool;
	if(packet == NULL) return;

	pool = packet->pool;
	if((packet == NULL)||(packet -> data == NULL) || pool == NULL) return; 
	
	pthread_mutex_lock(&pool->mt);

	count = jqueue_size(pool->cacheque_use);
	for(; i < count ; i++){
		p = jqueue_pull(pool->cacheque_use);

		if(p == packet){
			packet->len = 0;
			packet->data = (char*)packet->heap;
			jqueue_push(pool->cacheque_free,p,0);
			break;
	    }
		
		jqueue_push(pool->cacheque_use,p,0);
	}
	
	pthread_mutex_unlock(&pool->mt);

}

response_pkt_p response_pkt_clone(response_pkt_p pkt)
{
	response_pkt_pool_t pool= pkt->pool;
	response_pkt_p packet;
	if(pool == NULL || pkt == NULL) return NULL;

	pthread_mutex_lock(&pool->mt);

	packet = response_pkt_new(pool,pkt->len);
	packet->len  = pkt->len;
	memcpy(packet->data,pkt->data,pkt->len);

	pthread_mutex_unlock(&pool->mt);

	return packet;
}