#include "rptqueue.h"

#ifdef __cplusplus
extern "C"
{
#endif

/////////////////////////  posrpt  //////////////////////////////

position_t posrpt_new()
{
	position_t rtn = NULL;

	while ((rtn = (position_t)calloc(1, sizeof(position_st))) == NULL) Sleep(1);

	return rtn;
}

void  posrpt_free(position_t pos)
{
	if (pos == NULL) return;

	if (pos->userid)
		free(pos->userid);
	if (pos->bdid)
		free(pos->bdid);
	if (pos->towncode)
		free(pos->towncode);
	if (pos->time)
		free(pos->time);
	if (pos->lon)
		free(pos->lon);
	if (pos->lat)
		free(pos->lat);
	if (pos->hgt)
		free(pos->hgt);
	if (pos->tabletime)
		free(pos->tabletime);

	free(pos);
}

/////////////////////////  evtrpt  //////////////////////////////

event_t evtrpt_new()
{
	event_t rtn = NULL;

	while ((rtn = (event_t)calloc(1, sizeof(event_st))) == NULL) Sleep(1);

	return rtn;
}

void  evtrpt_free(event_t evtrpt)
{
	if (evtrpt == NULL) return;

	if (evtrpt->userid)
		free(evtrpt->userid);
	if (evtrpt->bdid)
		free(evtrpt->bdid);
	if (evtrpt->towncode)
		free(evtrpt->towncode);
	if (evtrpt->time)
		free(evtrpt->time);
	if (evtrpt->lon)
		free(evtrpt->lon);
	if (evtrpt->lat)
		free(evtrpt->lat);
	if (evtrpt->hgt)
		free(evtrpt->hgt);
	if (evtrpt->evttype)
		free(evtrpt->evttype);
	if (evtrpt->evttypname)
		free(evtrpt->evttypname);
	if (evtrpt->description)
		free(evtrpt->description);
	if (evtrpt->tabletime)
		free(evtrpt->tabletime);

	free(evtrpt);
}

/////////////////////////////  rptqueue  /////////////////////////

rptqueue_t rptqueue_new()
{
	rptqueue_t rtn = NULL;

	while ((rtn = (rptqueue_t)calloc(1, sizeof(rptqueue_st))) == NULL) Sleep(1);

	rtn->queue = jqueue_new();

	pthread_mutex_init(&rtn->mt, NULL);

	return rtn;
}

void  rptqueue_free(rptqueue_t q)
{
	if (q == NULL) return;

	if (q->queue) jqueue_free(q->queue);
	pthread_mutex_destroy(&q->mt);

	free(q);
}

int rptqueue_getsize(rptqueue_t q)
{
	int size = 0;

	if (q == NULL) return;

	pthread_mutex_lock(&q->mt);

	size = jqueue_size(q->queue);

	pthread_mutex_unlock(&q->mt);

	return size;
}

void rptqueue_putpos(rptqueue_t q, position_t posrpt)
{
	if (q == NULL || posrpt == NULL) return;

	pthread_mutex_lock(&q->mt);

	jqueue_push(q->queue, posrpt, 0);

	pthread_mutex_unlock(&q->mt);
}

position_t   rptqueue_checkpos(rptqueue_t q)
{
	position_t pos = NULL;

	if (q == NULL) return NULL;

	pthread_mutex_lock(&q->mt);

	if (jqueue_size(q->queue) > 0){
		//pos = jqueue_pull(q->queue);
		pos = jqueue_first(q->queue);	// 查看首元素？
	}

	pthread_mutex_unlock(&q->mt);

	return pos;
}

position_t   rptqueue_pullpos(rptqueue_t q)
{
	position_t pos = NULL;

	if (q == NULL) return NULL;

	pthread_mutex_lock(&q->mt);

	if (jqueue_size(q->queue) > 0){
		pos = jqueue_pull(q->queue);
	}

	pthread_mutex_unlock(&q->mt);

	return pos;
}

void  rptqueue_putevt(rptqueue_t q, event_t evtrpt)
{
	if (q == NULL || evtrpt == NULL) return;

	pthread_mutex_lock(&q->mt);

	jqueue_push(q->queue, evtrpt, 0);

	pthread_mutex_unlock(&q->mt);

}

event_t   rptqueue_checkevt(rptqueue_t q)
{
	event_t evt = NULL;

	if (q == NULL) return NULL;

	pthread_mutex_lock(&q->mt);

	if (jqueue_size(q->queue) > 0){
		evt = jqueue_first(q->queue);
	}

	pthread_mutex_unlock(&q->mt);

	return evt;
}

event_t   rptqueue_pullevt(rptqueue_t q)
{
	event_t evt = NULL;

	if (q == NULL) return NULL;

	pthread_mutex_lock(&q->mt);

	if (jqueue_size(q->queue) > 0){
		evt = jqueue_pull(q->queue);
	}

	pthread_mutex_unlock(&q->mt);

	return evt;
}

#ifdef __cplusplus
}
#endif