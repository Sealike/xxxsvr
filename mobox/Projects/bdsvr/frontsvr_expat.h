#ifndef __FRONTSVR_EXPAT_H__
#define __FRONTSVR_EXPAT_H__

#include "expat.h"

#ifdef __cplusplus
extern "C"
{
#endif
typedef struct _frontsvr_exp_st{
	struct _exp_st _base;
	int   indlen;
	uchar checksum;
}frontsvr_exp_st, *frontsvr_exp_t;

#define _BASE(p) p->_base

frontsvr_exp_t frontsvr_exp_new(void* param);
void         frontsvr_exp_free(frontsvr_exp_t);

#ifdef __cplusplus
}
#endif
#endif

