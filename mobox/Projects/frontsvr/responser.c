#include <stdlib.h>
#include <assert.h>
#include <util/util.h>
#include "responser.h"
#include "tool.h"

static int _responser_read_data(responser_t responser, char *buff, int buffl) 
{
	int len  = 0;
	int port = 0;
	char * ip = NULL;
	struct sockaddr_in from;
	int fromlen = sizeof(from);
	
	log_debug(ZONE, "reading from udp socket fd %d", responser->recv_fd->fd);

	/* do the read */
	len = recvfrom(responser->recv_fd->fd, buff, buffl, 0, (struct sockaddr *)&from, &fromlen);
	if(len == -1) 
	{
		return 0;				
	}

	port = j_inet_getport((struct sockaddr_storage *)&from);
	ip = inet_ntoa(from.sin_addr);

	log_debug(ZONE, "received data from %s %d bytes\n", ip, len);

	return len;
}

responser_t	responser_new(config_t config, responser_callback_t data_cb, void *data_cb_para, client_p client, long cid) 
{
	responser_t responser = NULL;
	if(client == NULL) return NULL;

	while(NULL == (responser = (responser_t)malloc(sizeof(struct _responser_st)))) sleep(1);
	memset(responser, 0, sizeof(struct _responser_st));

	responser->client    = client;
	responser->pop_data_fun = data_cb;
	responser->pop_data_para = data_cb_para;
	responser->local = "127.0.0.1";
	responser->local_port = 0;
	responser->last_measure = time(NULL);
	responser->measure_interval = 0;

	return responser;
}

int responser_can_read(responser_t responser)
{
	int read = 0, i = 0, pop = 1;
	log_debug(ZONE, "responser ready for reading");

	/* get them to read stuff */
	read =  _responser_read_data(responser, responser->recv_buff, sizeof(responser->recv_buff));

	if(read > 0)
	{
		/* into the parser with you */		
		if(pop)
		{
			if(responser->pop_data_fun) responser->pop_data_fun(responser->recv_buff, read, responser->pop_data_para);
		}
	}

	return 1;
}

int responser_mio_callback(mio_t m, mio_action_t a, mio_fd_t fd, void *data, void *arg) {
	responser_t responser = (responser_t)arg;
	switch(a) {
	case action_READ:
		return responser_can_read(responser);
	default:
		return 0;
	}

	return 0;
}

void responser_write(responser_t responser,const char *data, int datal)
{
	int len = 0;
	int pop = 1;
	struct sockaddr_storage sa;
	memset(&sa, 0, sizeof(sa));
	
	if(pop)
	{
		if(responser->pop_data_fun) 
			responser->pop_data_fun((void*)data, datal, responser->pop_data_para);
	}
	return;
	/* if we specified an ip to bind to */
	if(responser->local != NULL && !j_inet_pton(responser->local, &sa))
		return ;

	if(sa.ss_family == 0)
		sa.ss_family = AF_INET;

	j_inet_setport(&sa, responser->local_port);

	len = sizeof(struct sockaddr_in);
	log_debug(ZONE, "write to [%s:%d] data length [%d]", responser->local, responser->local_port, datal);

	sendto(responser->send_fd->fd, data, datal, 0, (struct sockaddr*)&sa, len);
}

int responser_stop(responser_t responser) {
	return 0;	
}

void responser_free(responser_t responser) {
	
	assert(responser);
	if(NULL == responser) return;
		
	if(responser->send_fd) 
	{
		mio_udp_close(responser->send_fd);
		free(responser->send_fd);
	}	

	free(responser);

	return;
}
