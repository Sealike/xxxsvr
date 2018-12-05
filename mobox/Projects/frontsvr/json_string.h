#ifndef _JSON_STRING_H__
#define _JSON_STRING_H__

//#include "json/cJSON.h"				// 该头文件是jabber库中的一个
//#include "subst/subst.h"



#define  DYNCODE	0
#define  CONFIGACK	1

//typedef enum 
//{
//	DYNCODE = 0,
//	CONFIGACK
//}json_string_t;

typedef struct _dyncode_st
{
	char * taskid;
	char * executor;
	char * rslt;
	char * net;
}dyncode_st, *dyncode_t;


typedef struct _configack_st
{
	char * account;
	char * rslt;
	char * err;
	char * seqno;
}configack_st, *configack_t;


//char* json_string(void* arg, int type);

#endif