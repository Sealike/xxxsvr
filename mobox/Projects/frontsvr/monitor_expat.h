
#ifndef __MONITOR_EXPAT_H__
#define __MONITOR_EXPAT_H__
#include "expat.h"

typedef struct _monitor_exp_st{
	struct _exp_st _base;
	int   indlen;
	uchar checksum;
}monitor_exp_st, *monitor_exp_t;

#define _BASE(p) p->_base

monitor_exp_t monitor_exp_new(void* param);
void      monitor_exp_free(monitor_exp_t);

#endif

