#ifndef _GLOBALDATA_H__
#define _GLOBALDATA_H__

#include "util/util.h"

typedef struct  _epro_st{
	char fromserver[64];
	char toserver[64];
}epro_st,*epro_t;

epro_t epro_new();
void    epro_free(epro_t);

unsigned char checksum(unsigned char buffer[], int len);
char* UTF8toGBK(char *pText);
char* usernotify_jsonstring(int *fromid, int toid, int seqno);

typedef struct _auth_st{
	int ok;
	char peername[64];
	char err[128];
	int  seqno;
}auth_st, *auth_t;

typedef struct _init_sess_st{
	char to[64];
	char from[64];
}init_sess_st, *init_sess_t;

typedef struct _init_ack_st{
	char to[64];
	char from[64];
	char role[64];
	int  ifok;
	int  seqno;
	char err[512];
}init_ack_st, *init_ack_t;

typedef struct _pong_st{
	char to[64];
	char from[64];	
}pong_st, *pong_t;

#endif