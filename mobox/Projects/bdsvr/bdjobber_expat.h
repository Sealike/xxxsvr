
#ifndef __BDJOBBER_DEVEXPAT__H__
#define __BDJOBBER_DEVEXPAT__H__
#include "expat.h"

typedef struct _bdjobber_exp_st{
	struct _exp_st _base;
	int   indlen;
	uchar checksum;
}bdjobber_exp_st,*bdjobber_exp_t;

#define _BASE(p) p->_base

bdjobber_exp_t bdjobber_exp_new(void* param);
void         bdjobber_exp_free(bdjobber_exp_t);

#endif

