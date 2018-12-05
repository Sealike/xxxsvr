#ifndef __FINDJOBBER_H__
#define __FINDJOBBER_H__
#include "client.h"
#ifdef __cplusplus
extern "C"
{
#endif
sess_t getrefsvr(const char* cname);
sess_t getrefclnt(const char* cname);
#ifdef __cplusplus
}
#endif
#endif