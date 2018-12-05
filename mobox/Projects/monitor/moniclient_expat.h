
#ifndef __MONICLIENT_EXPAT_H__
#define __MONICLIENT_EXPAT_H__
#include "expat.h"

typedef struct _moniclient_exp_st{
	struct _exp_st _base;
	int   indlen;
	uchar checksum;
}moniclient_exp_st, *moniclient_exp_t;

#define _BASE(p) p->_base

moniclient_exp_t moniclient_exp_new(void* param);
void      moniclient_exp_free(moniclient_exp_t);

#endif

