#pragma once
#include "http/httpc.h"
#include "pthreads/pthread.h"

typedef struct _httpclient_st{
	httpc_t httpc;
	pthread_t worker;
	char    * https_ip;
	int       https_port;
	int       shutdown;
	HANDLE    evtstop;
}httpclient_st, *httpclient_t;

httpclient_t httpclient_new(char* ip,int port);

void         httpclient_free(httpclient_t client);

int         httpclient_start(httpclient_t client);

void         httpclient_stop(httpclient_t client);