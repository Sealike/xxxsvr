#include "jobqueue.h"

bdjob_t bdjob_new()
{
	bdjob_t rtn = NULL;

	while ((rtn = (bdjob_t)calloc(1, sizeof(bdjob_st))) == NULL) Sleep(1);

	return rtn;

}

void    bdjob_free(bdjob_t bdjob)
{
	if (bdjob) free(bdjob);
}


jobqueue_t jobqueue_new()
{
	jobqueue_t rtn = NULL;

	while ((rtn = (jobqueue_t )calloc(1, sizeof(jobqueue_st))) == NULL) Sleep(1);

	rtn->queue = jqueue_new();
	 
	pthread_mutex_init(&rtn->mt, NULL);

	return rtn;

}

void  jobqueue_free(jobqueue_t jobq)
{
	if (jobq == NULL) return;

	if (jobq->queue) jqueue_free(jobq->queue);
	pthread_mutex_destroy(&jobq->mt);

	free(jobq);

}

bdjob_t jobqueue_getjob(jobqueue_t jobq)
{
	bdjob_t job = NULL;

	if (jobq == NULL) return NULL;

	pthread_mutex_lock(&jobq->mt);

	if (jqueue_size(jobq->queue) > 0){
		job = jqueue_pull(jobq->queue);
	}

	pthread_mutex_unlock(&jobq->mt);

	return job;
}

void  jobqueue_putjob(jobqueue_t jobq, bdjob_t job)
{
	if (jobq == NULL || job == NULL) return;

	pthread_mutex_lock(&jobq->mt);

	jqueue_push(jobq->queue, job, 0);

	pthread_mutex_unlock(&jobq->mt);

	return job;
}