#ifndef _WARNSVR_CALLBACK_H__
#define _WARNSVR_CALLBACK_H__
#ifdef __cplusplus
extern "C"
{
#endif

	void  warnsvr_exp_cb(int msgid, void* msg, int len, void* param);

#ifdef __cplusplus
}
#endif

#include "client.h"
#include "p2p.h"


#endif