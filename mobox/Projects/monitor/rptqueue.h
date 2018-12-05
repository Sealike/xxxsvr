#ifndef _RPTQUEUE_H__
#define _RPTQUEUE_H__

#include "dbaccess.h"

#ifdef __cplusplus
extern "C"
{
#endif


/////////////////////////  posrpt  //////////////////////////////

position_t posrpt_new();
void	   posrpt_free(position_t posrpt);

/////////////////////////  evtrpt  //////////////////////////////

event_t evtrpt_new();
void     evtrpt_free(event_t evtrpt);

///////////////////////////   rptqueue  //////////////////////////////////

typedef struct _rptqueue_st{
	jqueue_t queue;
	pthread_mutex_t mt;
}rptqueue_st, *rptqueue_t;

rptqueue_t	rptqueue_new();
void		rptqueue_free(rptqueue_t q);

int rptqueue_getsize(rptqueue_t q);

void		rptqueue_putpos(rptqueue_t q, position_t posrpt);
position_t   rptqueue_pullpos(rptqueue_t q);
position_t   rptqueue_checkpos(rptqueue_t q);
//position_t	rptqueue_getpos(rptqueue_t q);

void		rptqueue_putevt(rptqueue_t q, event_t evtrpt);
event_t		rptqueue_checkevt(rptqueue_t q);
event_t		rptqueue_pullevt(rptqueue_t q);
//event_t		rptqueue_getevt(rptqueue_t q);



#ifdef __cplusplus
}
#endif


#endif