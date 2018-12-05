#include "expat.h"

#ifndef __TOWNEXPAT__H__
#define __TOWNEXPAT__H__

typedef struct _town_exp_st{
	struct _exp_st _base;
	uchar checksum;
	int   indlen;
}town_exp_st, *town_exp_t;

#define _BASE(p) p->_base

town_exp_t town_exp_new(void* param);
void       town_exp_free(town_exp_t);

#endif