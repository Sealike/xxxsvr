#include "p2pclient.h"
#include "responsepkt.h"
#include "protobuffer.h"
#include "instapp.h"
#include "instapp.h"
#define P2P_MAX_MTU  (64*1024)
#define P2P_INIT_SESSION  0x10



static int _p2pclient_mio_callback(mio_t m, mio_action_t a, mio_fd_t fd, void *data, void *arg);

log_t logger()
{
	return g_instapp->log;
}

void _p2pclient_init(p2pclient_t p2p) 
{
	int count = 0;
	int check = 0;
	int endpos = 0;

	log_debug(ZONE, "doing server init for p2p %d", p2p->sess.fd->fd);
	mio_read(p2p->mio, p2p->sess.fd);
	log_debug(ZONE, "waiting for p2p CMD");
	p2p->want_read = 1;

	protobuffer_clnt_send(p2p,eProto_initsession,NULL); //与服务器进行初始化连接
}

void _p2pclient_release(p2pclient_t p2p)
{
	if(p2p){
		if(p2p->rcmdq){
			response_pkt_p p = NULL;
			while(p = jqueue_pull(p2p->rcmdq)){
				response_pkt_free(p);
			}
			jqueue_free(p2p->rcmdq);
			p2p->rcmdq = NULL;
		}

		if(p2p->wbufq){
			response_pkt_p p = NULL;
			while(p = jqueue_pull(p2p->wbufq)){
				response_pkt_free(p);
			}

			jqueue_free(p2p->wbufq);
			p2p->wbufq = NULL;
		}

		if(p2p->wbufpending){
			response_pkt_free(p2p->wbufpending);
			p2p->wbufpending = NULL;
		}
	}
}

BOOL p2pclient_connect(p2pclient_t p,mio_t mio,jqueue_t deadsess,void* param,sess_type_t sesstype,io_complete_cb iccb,void* para)
{
	static unsigned int conn_cnt = 0;
	instapp_t instapp = (instapp_t)param;

	if(deadsess) p->deadsess = deadsess;
	
	p->sess.fd = mio_connect(mio,p->port,p->ip,_p2pclient_mio_callback,p);
	if(p->sess.fd == NULL){
		if (logger()) log_write(logger(), LOG_ERR, "[%s, port=%d] failed to connect to %s.", p->ip, p->port, p->sess.sname);
		jqueue_push(deadsess,p,0);
		p->sess.binitsess = FALSE;	// 无论是否连接成功，都先将binitsess置为FALSE
		p->lost = TRUE;
		return FALSE;
	}else{
		p->mio = mio;
		snprintf(p->sess.skey,sizeof(p->sess.skey)-1,"%d",p->sess.fd->fd);
		j_inet_ioctl(p->sess.fd->fd, FIONBIO, 1);
		if (logger()) log_write(logger(), LOG_NOTICE, "[ip=%s, port=%d] success connect to %s.", p->ip, p->port, (conn_cnt==0)?"frontsvr":p->sess.sname);
		p->lost = FALSE;
		p->client = instapp;
		p->sess.instapp = instapp;
		p->sess.binitsess = FALSE;  // 无论是否连接成功，都先将binitsess置为FALSE
		p->iccb = iccb;
		p->para = para;
		p->sess.last_activity = time(0);

		jqueue_push(instapp->session_que,p,0);

		p2pclient_setup(p,sesstype);
		_p2pclient_init(p);

		if (++conn_cnt > 9999)
			conn_cnt = 1;

		return TRUE;
	}
}

void p2pclient_setup(p2pclient_t p,sess_type_t sesstype){

	p->sess.type = sesstype;

	if(p->wbufq == NULL)
		p->wbufq = jqueue_new();

	if(p->rcmdq == NULL)
		p->rcmdq = jqueue_new();

	if(p->pktpool == NULL)
		p->pktpool = response_pkt_pool_new();	

	p->max_queue = 1024;
}

p2pclient_t p2pclient_new(char* strip,int port,sess_type_t sesstype)
{
	p2pclient_t p = (p2pclient_t)calloc(1,sizeof(p2pclient_st));
	p->ip = _strdup(strip);
	p->port = port;
	p2pclient_setup(p,sesstype);

	return p;
}

void p2pclient_free(p2pclient_t p)
{
	if(p)
	{
		_p2pclient_release(p);

		free(p);
	}

}

static void _p2pclient_kill(p2pclient_t p2p) 
{
	if (p2p->sess.fd != NULL)
	mio_close(p2p->mio, p2p->sess.fd);
}

static int _p2pclient_parse(p2pclient_t p2p, char *buff, int len) 
{
	if(p2p->iccb){
		p2p->iccb(buff,len,p2p->para);
	}

	if(p2p->expat){
		exp_process_data(p2p->expat,buff,len);
	}

	return 0;
}

void p2pclient_close(p2pclient_t p2p)
{
	_p2pclient_kill(p2p);
}

int p2pclient_write(p2pclient_t p2p, response_pkt_p packet) {

	if(p2p->wbufq == NULL) return -1;

	jqueue_push(p2p->wbufq, packet, 0);

	p2p->want_write = 1;

	if(p2p->mio == NULL || p2p->sess.fd == NULL) return -1;
	
	mio_write(p2p->mio, p2p->sess.fd);

	if(p2p->want_read) mio_read(p2p->mio, p2p->sess.fd);

	return 0;
}

static int _p2pclient_can_read(p2pclient_t p2p) 
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

		_p2pclient_kill(p2p);

		return 0;
	}else if(len == 0) {
		/* they went away */
		_p2pclient_kill(p2p);

		return 0;
	}

	log_debug(ZONE, "read %d bytes", len);
	if (len > 50)
	{
		if (len > P2P_MAX_MTU)
		{
			buffer[P2P_MAX_MTU - 1] = '\0';
		}
		else
		{
			buffer[len] = '\0';
		}
		log_debug(ZONE, "p2p_client recv %d bytes,buffer = %s", len,buffer);
	}
	_p2pclient_parse(p2p, buffer, len);

	/*while((in = jqueue_pull(p2p->rcmdq)) != NULL) 
	{
		log_debug(ZONE, "p2p cmd :[%.*s]", in->len, in->data);
		if(0 != (request_parse(in)))
			_p2pclient_kill(p2p);
		_sx_buffer_free(in);
	}*/
	if(p2p->want_read) mio_read(p2p->mio, p2p->sess.fd);
	return 1;
}

/** we can write */
static int _p2pclient_get_pending_write(p2pclient_t p2p) 
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

static int _p2pclient_io_write(p2pclient_t p2p, response_pkt_p buf)
{
	int len;

	len = send(p2p->sess.fd->fd, buf->data, buf->len, 0);
	if(len >= 0) {                
		return len;            
	}

	if(MIO_WOULDBLOCK) 
		return 0;

	_p2pclient_kill(p2p);

	return -1;
}

static int _p2pclient_can_write(p2pclient_t p2p)
{
	response_pkt_p out;
	int ret, written;

	assert((int) (p2p != NULL));

	/* do we care? */
	if(!p2p->want_write)
		return 0;           /* no more thanks */

	ret = _p2pclient_get_pending_write(p2p);
	if (ret < 0) {
		/* permanent error so inform the app it can kill us */
		_p2pclient_kill(p2p);
		return 0;
	}

	/* if there's nothing to write, then we're done */
	if(p2p->wbufpending == NULL) {
		if(p2p->want_read) mio_read(p2p->mio, p2p->sess.fd);
		return p2p->want_write;
	}

	out = p2p->wbufpending;
	p2p->wbufpending = NULL;

	/* get the callback to do the write */
	written = _p2pclient_io_write(p2p, out);

	if(written < 0) {
		/* bail if something went wrong */
		response_pkt_free(out);
		p2p->want_write = 0;
		_p2pclient_kill(p2p);
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

	if(p2p->want_read) mio_read(p2p->mio, p2p->sess.fd);
	return p2p->want_write;
}

int on_read_action(mio_fd_t mfd,p2pclient_t p2p)
{
	int nbytes = 0;
	log_debug(ZONE, "p2p read action on fd %d", mfd->fd);

	/* they did something */
	p2p->sess.last_activity = time(NULL);
#ifdef _WIN32			
	ioctlsocket(mfd->fd, FIONREAD, &nbytes);
#else
	ioctl(mfd->fd, FIONREAD, &nbytes);
#endif	
	if(nbytes == 0) {
		log_debug(ZONE, " recv p2p[%d] closed event", mfd->fd);
		_p2pclient_kill(p2p);
		return 0;
	}

	return _p2pclient_can_read(p2p);
}

int on_write_action(mio_fd_t fd,p2pclient_t p2p)
{
	log_debug(ZONE, "write action on fd %d", fd->fd);
	return _p2pclient_can_write(p2p);
}

int on_close_action(mio_fd_t fd,p2pclient_t p2p)
{	
	log_debug(ZONE, "close action on fd %d", fd->fd);

	_p2pclient_release(p2p);

	if(p2p->deadsess){
		jqueue_push(p2p->deadsess,(void*)p2p,0);
	}

	return 1;
}


int on_accept_action(mio_t m,mio_fd_t fd,void* data,void* arg)
{	
	p2pclient_t p2p = NULL;
	instapp_t   instapp = (instapp_t) arg;
	struct sockaddr_storage sa;
	int namelen = sizeof(sa), port, flags = 0;

	log_debug(ZONE, "accept action on fd %d", fd->fd);

	getpeername(fd->fd, (struct sockaddr *) &sa, &namelen);
	port = j_inet_getport(&sa);

	if(logger())
		log_write(logger(), LOG_NOTICE, "[%d] [%s, port=%d] connect", fd->fd, (char *) data, port);

	if(0) {
		log_debug(ZONE, "p2p denied by configuration [%d] [%s, port=%d] connect", fd->fd, (char *) data, port);
		return 1;
	}

	j_inet_ioctl(fd->fd, FIONBIO, 1);

	p2p = (p2pclient_t) calloc(1, sizeof(p2pclient_st));

	p2p->sess.fd = fd;

	p2p->ip = j_strdup((char *) data);
	p2p->port = port;

	/* they did something */
	p2p->sess.last_activity = time(NULL);		

	/* find out which port this is */
	getsockname(fd->fd, (struct sockaddr *) &sa, &namelen);
	port = j_inet_getport(&sa);

	/* remember it */
	sprintf(p2p->sess.skey, "%d", fd->fd);
	//xhash_put(client->sessions, p2p->sess.skey, (void *) p2p);
	//jqueue_push(client->session_que,(void *) p2p,0);
	p2p->sess.type = front_SESS;

	assert(p2p->wbufq == NULL);
	p2p->wbufq = jqueue_new();

	assert(p2p->rcmdq == NULL);
	p2p->rcmdq = jqueue_new();

	p2p->max_queue = instapp->stream_maxq;

	mio_app(m, fd, _p2pclient_mio_callback, (void *) p2p);

	//_p2pclient_init(p2p);

	return 0;
}

static int _p2pclient_mio_callback(mio_t m, mio_action_t a, mio_fd_t fd, void *data, void *arg) 
{	
	p2pclient_t p2p = (p2pclient_t) arg;

	struct sockaddr_storage sa;
	int namelen = sizeof(sa), flags = 0;

	switch(a) {
	case action_READ:		
		return on_read_action(fd,p2p);
	case action_WRITE:
		return on_write_action(fd,p2p);
	case action_CLOSE:		
		return on_close_action(fd,p2p);
	case action_ACCEPT:
		return on_accept_action( m, fd,data,arg);
	}

	return 0;
}