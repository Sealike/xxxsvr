#ifndef __REDISCOMMAND__H_
#define __REDISCOMMAND__H_
#include "hiredis.h"
int        process_push_str(redisContext* c, char* key, char *value);
int        set_activetime(redisContext* c, char* key, int iseconds);//��������ʱ��
char*      process_get_str(redisContext* c, char* key);

#endif