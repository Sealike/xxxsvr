#ifndef __OPENSESSION_EXP_H__
#define __OPENSESSION_EXP_H__

#include "expat.h"
#ifdef __cplusplus
extern "C"
{
#endif
typedef struct _opensess_exp_st{
	struct _exp_st _base;
	int   indlen;
	uchar checksum;
}opensess_exp_st,*opensess_exp_t;

#define _BASE(p) p->_base

opensess_exp_t opensess_exp_new(void* param);
void           opensess_exp_free(opensess_exp_t);

#ifdef __cplusplus
}
#endif
#endif