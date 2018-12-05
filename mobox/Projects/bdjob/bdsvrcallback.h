#ifndef  _DEV_BDSVR_CALLBACK__H__
#define  _DEV_BDSVR_CALLBACK__H__
#include "bdexplain.h"
#include "BDDeviceArray.h"
#include "RemoteServce.h"
#ifdef __cplusplus
extern "C"
{
#endif

void  bdsvr_exp_cb(int msgid ,void* msg,int len,void* param);
#ifdef __cplusplus
}
#endif
#endif