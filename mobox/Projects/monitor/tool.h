#ifndef __TOOL__H__
#define __TOOL__H__
#include "mio/mio.h"

#define NULL     0
#define HEADLEN  12
#define FALSE    0
#define TRUE     1

enum {
	PACK_UNSEARCHED = 0,
	PACK_SEARCHED,
	PACK_STILLDATA,
	PACK_UNFINISHED,
	PACK_FINISHED,
};

enum {
	SEARCHMACHINE_NULL = 0,
	SEARCH_GETHEAD,
	SEARCH_GETDATA,
	SEARCH_GETCHECK,
	SEARCHMACHINE_BEGININD,
	SEARCHMACHINE_IDENTIFIER,
	SEARCHMACHINE_LENGTH,
	SEARCHMACHINE_BINARYCHECK,
};

#define ISLCHAR(w)     (w>='a'&&w<='z')
#define ISHCHAR(w)     (w>='A'&&w<='Z')
#define ISDIGIT(w)     (w>='0'&&w<='9')
#define ISCHAR(w)       (ISDIGIT(w)|| ISLCHAR(w)||ISHCHAR(w))

#define NULLReturn(condition , rtnval)      {if((condition) == NULL)  return rtnval;}
#define FALSEReturn(condition, rtnval)      {if((condition) == FALSE) return rtnval;}
#define TRUEReturn(condition , rtnval)      {if((condition) == TRUE)  return rtnval;}

typedef struct _SMsgTitle_st{
	int id;
	char title[32];
}SMsgTitle_st, *SMsgTitle_t;


mio_fd_t mio_udp_client(int maxbufferszie);
void     mio_udp_close(mio_fd_t fd);

typedef struct _io_throughout_check_st{
	int count;
	int memcnt;
	time_t tick;
	int64_t memtick;
	int timeouts;
	int throughout;
}io_throughout_check_st,*io_throughout_check_t;

io_throughout_check_t io_throughout_check_new(int timeouts);
int                   io_throughout_check_rslt(io_throughout_check_t checker);
BOOL                  io_throughout_check(io_throughout_check_t checker,int count,int printit);
void                  io_throughout_check_free(io_throughout_check_t checker);

typedef struct _ratecheck_st{
	int total;
	int waitmsecs;
	int count;
	long long last;
}ratecheck_st,*ratecheck_t;

ratecheck_t ratecheck_new(int total,int timeouts);
BOOL ratecheck_add(ratecheck_t,int count);
int  ratecheck_left(ratecheck_t);
void ratecheck_reset(ratecheck_t);
int  ratecheck_count(ratecheck_t);

#define _arraysize(_arr__) (sizeof(_arr__)/sizeof(_arr__[0]))

typedef struct _SMsgTitle_st2{
	int id;
	char title[32];
}SMsgTitle_st2, *SMsgTitle_t2;

#endif