#include "expat.h"
#ifndef __WARNSVR_EXPAT__H__
#define __WARNSVR_EXPAT__H__
#ifdef __cplusplus
extern "C"
{
#endif
	typedef struct _warnsvr_exp_st{
		struct _exp_st _base;
		int   indlen;
		uchar checksum;
	}warnsvr_exp_st, *warnsvr_exp_t;

#define _BASE(p) p->_base

	warnsvr_exp_t warnsvr_exp_new(void* param);
	void          warnsvr_exp_free(warnsvr_exp_t);

#ifdef __cplusplus
}
#endif
#endif

