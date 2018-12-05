#ifndef __SEARCHEXP__H_
#define __SEARCHEXP__H_

#ifdef __cplusplus
extern "C"
{
#endif
typedef unsigned char uchar;

#define UNPACKBUFFSIZE  (2048)

enum{
	cunpack_initsession = 100,
	cunpack_ping,
	cunpack_pong,
	cunpack_message,
	cunpack_txa,//Õ®–≈«Î«Û
	cunpack_ide
};

typedef void  (*pfunsearchexp_cb)(int msgid ,void* msg,int len,void* param);
typedef int   (*search_t)(void* exp,uchar*buffer,int len,int* searchpos);
typedef int   (*explain_t)(void* exp,uchar*frame,int len);

typedef struct _exp_st{
	char   frame[UNPACKBUFFSIZE];
	int    framelen;	
	int    searchstatus;		
	uchar  data[UNPACKBUFFSIZE];
	uchar  Head[64];
	int    HeadPtr;
	pfunsearchexp_cb m_cb;
	search_t  search;
	explain_t explain;
	void* param;
}_exp_st,*_exp_t;

void      exp_process_data(_exp_t,char* data,int len);
void      exp_set_callback(_exp_t,pfunsearchexp_cb pfunc);
#ifdef __cplusplus
}
#endif

#endif