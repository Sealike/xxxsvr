#include "expat.h"

#ifndef __BACKEXPAT__H__
#define __BACKEXPAT__H__

typedef struct _back_exp_st{
	struct _exp_st _base;
	uchar checksum;
	int   indlen;
}back_exp_st,*back_exp_t;

#define _BASE(p) p->_base

back_exp_t back_exp_new(void* param);
void       back_exp_free(back_exp_t);

#endif