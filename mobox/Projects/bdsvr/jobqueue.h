#pragma once
#include "pthreads/pthread.h"
#include "util/util.h"
#include "json/cJSON.h"

#ifdef __cplusplus
extern "C"
{
#endif



typedef struct _bdjob_st{
	char type[15];
	char content[2048];
}bdjob_st, *bdjob_t;

bdjob_t bdjob_new();
void    bdjob_free(bdjob_t);

//////////////////////////////////////////////////////////////////////////

typedef struct _jobqueue_st{	
	jqueue_t queue;
	pthread_mutex_t mt;
}jobqueue_st, *jobqueue_t;

jobqueue_t jobqueue_new();
void       jobqueue_free(jobqueue_t);

bdjob_t    jobqueue_getjob(jobqueue_t);
void       jobqueue_putjob(jobqueue_t, bdjob_t job);


#ifdef __cplusplus
}
#endif
