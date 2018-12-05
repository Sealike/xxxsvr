#include "expat.h"
#ifndef __DEVEXPAT__H__
#define __DEVEXPAT__H__

typedef struct _router_exp_st{
	struct _exp_st _base;
	int   indlen;
	uchar checksum;
}router_exp_st, *router_exp_t;

#define _BASE(p) p->_base

router_exp_t router_exp_new(void* param);
void         router_exp_free(router_exp_t);

#endif

