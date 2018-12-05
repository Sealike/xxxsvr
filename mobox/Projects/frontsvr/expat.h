#ifndef __SEARCHEXP__H_
#define __SEARCHEXP__H_
typedef unsigned char uchar;

#include "tool.h"


#define UNPACKBUFFSIZE  (2048*1024)

enum{
	cunpack_initsession = 100,
	cunpack_initack,
	cunpack_auth,
	cunpack_authack,
	cunpack_ping,
	cunpack_pong,
	cunpack_message,
	cunpack_position,
	cunpack_warning,
	cunpack_log,
	cunpack_reqcode,
	cunpack_dyncode,
	cunpack_taskpau,
	cunpack_taskcon,
	cunpack_taskdone,
	cunpack_status,
	cunpack_awake,
	cunpack_verify,
	cunpack_config,
	cunpack_remoteunlock,
	cunpack_rptctrl
};


typedef void(*pfunsearchexp_cb)(int msgid, void* msg, int len, void* param);
typedef int (*search_t)(void* exp,uchar*buffer,int len,int* searchpos);
typedef int (*explain_t)(void* exp,uchar*frame,int len);

typedef struct _exp_st{
	char  frame[UNPACKBUFFSIZE];
	int   framelen;
	int   searchstatus;
	uchar  data[UNPACKBUFFSIZE];
	uchar  Head[HEADLEN];
	int   HeadPtr;
	pfunsearchexp_cb m_cb;
	search_t  search;
	explain_t explain;
	void* param;
}_exp_st,*_exp_t;

void      exp_process_data(_exp_t,char* data,int len);
void      exp_set_callback(_exp_t,pfunsearchexp_cb pfunc);
void      exp_clear(_exp_t);

#endif