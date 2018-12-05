#include "tool.h"

mio_fd_t mio_udp_client(int maxbufferszie)
{
	mio_fd_t rtn = NULL;
#ifdef _WIN32
	rtn = (mio_fd_t)calloc(1,sizeof(struct mio_fd_st));
	rtn->fd = socket(AF_INET, SOCK_DGRAM, 0);
	j_inet_setsock_bufsize(rtn->fd , maxbufferszie);
#endif
	return rtn;

}

void     mio_udp_close(mio_fd_t fd)
{
	if(fd && fd->fd)
		closesocket(fd->fd);
}

io_throughout_check_t io_throughout_check_new(int timeouts)
{
	io_throughout_check_t p ;

	while((p = malloc(sizeof(io_throughout_check_st))) == NULL) Sleep(1);

	p->count = p->memcnt  = 0;
	p->tick  = p->memtick = 0;
	p->timeouts = timeouts;
	p->throughout = 0;

	return p;
}

BOOL io_throughout_check(io_throughout_check_t checker,int count,int printit)
{
	BOOL rslt = FALSE;

	if(checker->memtick == 0 && checker->tick==0)
		checker->memtick = checker->tick = time(NULL);

	checker->count += count;
	checker->tick = time(NULL);

	if(checker->tick - checker->memtick > checker->timeouts)
	{	
		checker->throughout = (checker->count - checker->memcnt)/(checker->tick - checker->memtick);
		checker->memtick = checker->tick  = time(NULL);
		checker->memcnt  = checker->count = 0;
		rslt = TRUE;
	}

	if(rslt && printit)		
		printf("throughoutput: %d bytes/s \n",checker->throughout);

	return rslt;
}

int  io_throughout_check_rslt(io_throughout_check_t checker)
{
	return checker->throughout;
}

void  io_throughout_check_free(io_throughout_check_t checker)
{
	if(checker) free(checker);

}

ratecheck_t ratecheck_new(int total,int timeoutms)
{
	ratecheck_t p;

	while((p = malloc(sizeof(ratecheck_st))) == NULL) Sleep(1);

	p->total = total;
	p->waitmsecs = timeoutms;
	p->count = 0;
	p->last = 0;
	return p;
}

BOOL ratecheck_add(ratecheck_t check,int count)
{
	if(check == NULL) return FALSE;

	check->count += count;
	
	if(check->count >= check->total) return TRUE;

	if( GetTickCount()-check->last >= check->waitmsecs ) return TRUE;

	return FALSE;
}

int  ratecheck_left(ratecheck_t check)
{
	if(check == NULL) return 0;

	return check->total - check->count;
}

void ratecheck_reset(ratecheck_t check)
{
	if(check == NULL) return ;

	check->count = 0;

	check->last = GetTickCount();
}

int  ratecheck_count(ratecheck_t ratechecker)
{
	return ratechecker->count;
}