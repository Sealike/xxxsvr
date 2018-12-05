#ifndef _GET_REF_SVR_H__
#define _GET_REF_SVR_H__



//#include "json/cJSON.h"
#include "client.h"

sess_t getrefsvr(const char* cname);
int sess_rename(sess_t sess, const char * newname, int len);






#endif