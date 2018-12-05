#include "p2p.h"
#include "client.h"
#include "responsepkt.h"
#include "router_expat.h"
#include "opensess_exp.h"

#define P2P_MAX_MTU  (4*1024)
extern int g_shutdown;
static int _p2p_mio_callback(mio_t m, mio_action_t a, mio_fd_t fd, void *data, void *arg);

void p2p_listener_start(_s_client_p client) 
{
	client->frontend_fd = mio_listen(client->mio, client->frontend_port, client->frontend_ip, _p2p_mio_callback, (void *)client);
	if(client->frontend_fd == NULL) {
		g_shutdown = TRUE;
		log_write(client->log, LOG_ERR, "[%s, port=%d @%d] failed to listen front end.", client->frontend_ip, client->frontend_port, client->frontend_fd->fd);
	}else
		log_write(client->log, LOG_NOTICE, "[%s, port=%d @%d] listening for front end connections", client->frontend_ip, client->frontend_port, client->frontend_fd->fd);

	client->backend_fd = mio_listen(client->mio, client->backend_port, client->backend_ip, _p2p_mio_callback, (void *)client);
	if (client->backend_fd == NULL) {
		g_shutdown = TRUE;
		log_write(client->log, LOG_ERR, "[%s, port=%d @%d] failed to listen back end.", client->backend_ip, client->backend_port, client->backend_fd->fd);
	}
	else
		log_write(client->log, LOG_NOTICE, "[%s, port=%d @%d] listening for back end connections", client->backend_ip, client->backend_port, client->backend_fd->fd);


}

static void _p2p_kill(p2p_t p2p) 
{
	mio_close(p2p->sess.client->mio, p2p->sess.fd);
}


void p2p_kill(p2p_t p2p)
{	
	if (p2p->wflush){
		if (p2p->wbufpending!=NULL || jqueue_size(p2p->wbufq))
			p2p->waitclose = 1;
	}

	if (p2p->waitclose == 0)
		_p2p_kill(p2p);
}

static void xhash_walk_throgh_2(xht h, const char *key, void *val, void *arg)
{
	if(val != NULL)
	{	
		int rtn = 0;
		printf("%s-%x\n",key,val);
	}
}

static int _p2p_parser(p2p_t p2p, char *buff, int len) 
{
	if(p2p->expat){
		//xhash_walk(client()->namedend,xhash_walk_throgh_2,NULL);
		exp_process_data(p2p->expat,buff,len);
	}
	return 0;
}

int p2p_stream_push(p2p_t p2p, response_pkt_p packet) {

	jqueue_push(p2p->wbufq, packet, 0);

	p2p->want_write = 1;

	/*if (p2p->sess.client == NULL || p2p->sess.fd == NULL || p2p->sess.client->mio == NULL)
		return 1;*/

	mio_write(p2p->sess.client->mio, p2p->sess.fd);

	if(p2p->want_read) mio_read(p2p->sess.client->mio, p2p->sess.fd);

	return 0;
}

int request_parse(sx_buf_t in)
{
	return 0;
}

static int _p2p_can_read(p2p_t p2p) 
{
	int len = 0;
	char buffer[P2P_MAX_MTU];

	log_debug(ZONE, "reading rmtp cmd from %d", p2p->sess.fd->fd);
	
	/* do the read */
	len = recv(p2p->sess.fd->fd, buffer, P2P_MAX_MTU, 0);	

	if(len < 0) {
		if(MIO_WOULDBLOCK) {
			return 1;
		}
		log_debug(ZONE, "[%d] [p2p] read error: %s (%d)", p2p->sess.fd->fd, MIO_STRERROR(MIO_ERROR), MIO_ERROR);

		_p2p_kill(p2p);

		return 0;
	}
	else if(len == 0) {
		/* they went away */
		_p2p_kill(p2p);

		return 0;
	}

	log_debug(ZONE, "read %d bytes", len);

	p2p->sess.last_activity = time(NULL);

	_p2p_parser(p2p, buffer, len);

	/*while((in = jqueue_pull(p2p->rcmdq)) != NULL) 
	{
	log_debug(ZONE, "p2p cmd :[%.*s]", in->len, in->data);
	if(0 != (request_parse(in)))
	_p2p_kill(p2p);
	_sx_buffer_free(in);
	}*/

	return 1;
}

/** we can write */
static int _p2p_get_pending_write(p2p_t p2p) 
{
	response_pkt_p out;	

	assert((int) (p2p != NULL));

	if (p2p->wbufpending != NULL) {
		/* there's already a pending buffer ready to write */
		return 0;
	}

	/* get the first buffer off the queue */
	out = jqueue_pull(p2p->wbufq);

	if(out == NULL) return 0;

	/* if there's more to write, we want to make sure we get it */
	p2p->want_write = jqueue_size(p2p->wbufq);

	if(p2p->want_write > p2p->max_queue) 
	{
		response_pkt_p packet = NULL;
		while(jqueue_size(p2p->wbufq) > 0)
			if(NULL != (packet = jqueue_pull(p2p->wbufq))) 
				response_pkt_free(packet);

		p2p->want_write = 0;
	}

	if (out->len == 0)
		/* if there's nothing to write, then we're done */
		response_pkt_free(out);
	else
		p2p->wbufpending = out;

	return 0;
}

static int _p2p_io_write(p2p_t p2p, response_pkt_p buf)
{
	int len;

	len = send(p2p->sess.fd->fd, buf->data, buf->len, 0);
	if(len >= 0) {                
		return len;            
	}

	if(MIO_WOULDBLOCK) 
		return 0;

	_p2p_kill(p2p);

	return -1;
}

static int _p2p_can_write(p2p_t p2p)
{
	response_pkt_p out;
	int ret, written;

	assert((int) (p2p != NULL));

	if (p2p->waitclose && jqueue_size(p2p->wbufq) == 0) 
		_p2p_kill(p2p);

	/* do we care? */
	if(!p2p->want_write)
		return 0;           /* no more thanks */

	ret = _p2p_get_pending_write(p2p);
	if (ret < 0) {
		/* permanent error so inform the app it can kill us */
		_p2p_kill(p2p);
		return 0;
	}

	/* if there's nothing to write, then we're done */
	if(p2p->wbufpending == NULL) {
		if(p2p->want_read) mio_read(p2p->sess.client->mio, p2p->sess.fd);
		return p2p->want_write;
	}

	out = p2p->wbufpending;
	p2p->wbufpending = NULL;

	/* get the callback to do the write */
	written = _p2p_io_write(p2p, out);

	if(written < 0) {
		/* bail if something went wrong */
		response_pkt_free(out);
		p2p->want_read = 0;
		p2p->want_write = 0;
		return 0;
	} else if(written < out->len) {
		/* if not fully written, this buffer is still pending */
		out->len -= written;
		out->data += written;
		p2p->wbufpending = out;
		p2p->want_write ++;

		log_debug(ZONE, "at (%d) sent %d bytes", p2p->sess.fd->fd, written);
	} else {
		log_debug(ZONE, "at (%d) sent %d bytes", p2p->sess.fd->fd, written);
		response_pkt_free(out);
		/* done with this */;
	}

	if(p2p->want_read) mio_read(p2p->sess.client->mio, p2p->sess.fd);
	return p2p->want_write;
}

static void _p2p_server_init(p2p_t p2p) 
{
	log_debug(ZONE, "doing server init for p2p %d", p2p->sess.fd->fd);
	mio_read(p2p->sess.client->mio, p2p->sess.fd);
	log_debug(ZONE, "waiting for p2p CMD");
	p2p->want_read = 1;

	//p2p_raw_write(p2p, RMTP_SERVER_VERSION, strlen(RMTP_SERVER_VERSION));
}

static int _p2p_mio_callback(mio_t m, mio_action_t a, mio_fd_t fd, void *data, void *arg) 
{
	sess_t sess = (sess_t) arg;
	_s_client_p client = (_s_client_p) arg;
	p2p_t p2p = (p2p_t) arg;
	sess_t quesess = NULL;
	struct sockaddr_storage sa;
	int namelen = sizeof(sa), port, nbytes, flags = 0;

	switch(a) {
	case action_READ:
		log_debug(ZONE, "p2p read action on fd %d", fd->fd);

		/* they did something */
		p2p->sess.last_activity = time(NULL);
#ifdef _WIN32			
		ioctlsocket(fd->fd, FIONREAD, &nbytes);
#else
		ioctl(fd->fd, FIONREAD, &nbytes);
#endif	
		if(nbytes == 0) {
			log_debug(ZONE, " recv p2p[%d] closed event", fd->fd);
			_p2p_kill(p2p);
			return 0;
		}
		//xhash_walk(client->namedend,xhash_walk_throgh_2,NULL);
		return _p2p_can_read(p2p);

	case action_WRITE:
		log_debug(ZONE, "write action on fd %d", fd->fd);
		return _p2p_can_write(p2p);

	case action_CLOSE:
		log_debug(ZONE, "close action on fd %d", fd->fd);

		log_write(p2p->sess.client->log, LOG_NOTICE, "-- close action on fd %d on sess %s --", fd->fd, sess->sname);

		//xhash_zap(p2p->sess.client->sessions, sess->sname);
		xhash_zap(p2p->sess.client->sessions, sess->sname);
		
		int size = jqueue_size(p2p->sess.client->session_que);
		for (int iter = 0; iter < size; iter++){
			quesess = jqueue_pull(p2p->sess.client->session_que);
			if (quesess == sess) break;
			jqueue_push(p2p->sess.client->session_que, quesess, 0);
		}

		//subject_del(p2p->sess.client->subjects, p2p->sess.subject, &p2p->sess);

		jqueue_push(p2p->sess.client->dead_sess, (void *) p2p, 0);

		break;

	case action_ACCEPT:
		log_debug(ZONE, "accept action on fd %d", fd->fd);

		getpeername(fd->fd, (struct sockaddr *) &sa, &namelen);
		port = j_inet_getport(&sa);

		log_write(client->log, LOG_NOTICE, "[%d] [%s, port=%d] connect", fd->fd, (char *) data, port);

		if(!client->enable_p2p) {
			log_debug(ZONE, "p2p denied by configuration [%d] [%s, port=%d] connect", fd->fd, (char *) data, port);
			return 1;
		}

		j_inet_ioctl(fd->fd, FIONBIO, 1);

		p2p = (p2p_t) calloc(1, sizeof(p2p_st));

		p2p->sess.client = client;

		p2p->sess.fd = fd;

		p2p->ip = j_strdup((char *) data);
		p2p->port = port;
		p2p->sess.state = state_init_SESS;
		/* they did something */
		p2p->sess.last_activity = time(NULL);		
		
		/* find out which port this is */
		getsockname(fd->fd, (struct sockaddr *) &sa, &namelen);
		//sprintf(p2p->sess.skey, "%d", fd->fd);

		port = j_inet_getport(&sa);
		
		if (port == client->frontend_port || port == client->backend_port){
			p2p->sess.type = back_server_SESS;
			p2p->expat = (_exp_t)opensess_exp_new(&p2p->sess);
		}

		jqueue_push(client->session_que,(void *) p2p,0);

		p2p->wbufq = jqueue_new();

		p2p->rcmdq = jqueue_new();		

		p2p->max_queue = client->stream_maxq;

		mio_app(m, fd, _p2p_mio_callback, (void *) p2p);

		_p2p_server_init(p2p);

		break;
	}

	return 0;
}