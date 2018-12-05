#include "expat.h"
#ifndef __DEVEXPAT__H__
#define __DEVEXPAT__H__
#ifdef __cplusplus
extern "C"
{
#endif
typedef struct _bdsvr_exp_st{
	struct _exp_st _base;
	int   indlen;
	uchar checksum;
}bdsvr_exp_st, *bdsvr_exp_t;

#define _BASE(p) p->_base

bdsvr_exp_t bdsvr_exp_new(void* param);
void         bdsvr_exp_free(bdsvr_exp_t);
#ifdef __cplusplus
}
#endif
#endif

