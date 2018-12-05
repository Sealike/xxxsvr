
#ifndef __BDSVR_EXPAT_H__
#define __BDSVR_EXPAT_H__
#include "expat.h"

typedef struct _bdsvr_exp_st{
	struct _exp_st _base;
	int   indlen;
	uchar checksum;
}bdsvr_exp_st, *bdsvr_exp_t;

#define _BASE(p) p->_base

bdsvr_exp_t bdsvr_exp_new(void* param);
void         bdsvr_exp_free(bdsvr_exp_t);

#endif

