#include "membuff.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#define NULL 0 
#define _init_mem_size (16);

static void _assure_mem_size(membuff_t mb,int addlen)
{
	if(mb == NULL || addlen<=0) return;

	if(mb->cap-mb->len <= addlen)
	{
		mb->cap = mb->len+addlen+( mb->cap>512*1024 ? mb->cap/3 : mb->cap );
		mb->data = realloc(mb->data,mb->cap);
	}
}

membuff_t membuff_new(int cap)
{
	membuff_t rtn = NULL;

	while((rtn = calloc(1,sizeof(membuff_st))) == NULL) Sleep(10);

	if(cap <= 0) cap = _init_mem_size;
	rtn->data = calloc(1,cap);
	rtn->cap = cap;

	rtn->len = 0;

	return rtn;
}

void membuff_free(membuff_t mb)
{
	if(mb == NULL) return;

	free(mb->data);

	free(mb);
}

int membuff_len(membuff_t mb)
{
	if(mb) return mb->len;

	return NULL;
}

int  membuff_cap(membuff_t mb)
{
	if(mb) return mb->cap;

	return 0;
}

membuff_t membuff_addstr(membuff_t mb,char* p,int len)
{
	if(mb == NULL || p == NULL || len<=0) return mb;

	_assure_mem_size(mb,len);

	memcpy((char*)mb->data+mb->len,p,len);

	mb->len+=len;

	return mb;
}

membuff_t membuff_addchar(membuff_t mb,char p)
{
	if(mb == NULL) return mb;

	_assure_mem_size(mb,1);
	mb->data[mb->len++] = p;

	return mb;
}

void  membuff_clear(membuff_t mb)
{
	if(mb) mb->len = 0;
}

char* membuff_data(membuff_t mb)
{
	if(mb) return mb->data;

	return NULL;
}

membuff_t membuff_add_printf(membuff_t mb,char* fmt,...)
{
	va_list ap;
	int printn;

	if(fmt == NULL || mb == NULL) return mb;

	va_start(ap,fmt);

	while((printn = vsnprintf(mb->data+mb->len,mb->cap-mb->len,fmt,ap))==mb->cap-mb->len || (printn <0 && errno==34))
	{
		_assure_mem_size(mb,(mb->len>1024*512) ? mb->cap/2 : mb->cap);
	}
	
	mb->len += printn;

	va_end(ap);

	return mb;
}