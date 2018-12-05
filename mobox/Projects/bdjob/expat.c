#include "expat.h"

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



#define HEADLEN  (4)
#define NULL     (0)


void  exp_set_callback(_exp_t exp,pfunsearchexp_cb pfunc)
{
	exp->m_cb  = pfunc;
}

void   exp_process_data(_exp_t exp,char* data,int len)
{
	int   curr_pos = 0;
	char* str = data;
	int   remain_len = len;	
	do{
		int status;		

		status = exp->search(exp,str,remain_len,&curr_pos);		

		if(status==PACK_UNSEARCHED || status==PACK_UNFINISHED)
			break;
		
		exp->frame[exp->framelen] = '\0';
		if (exp->explain(exp, exp->frame, exp->framelen) == -1) break;
		str = str + curr_pos+1;
		remain_len = remain_len - curr_pos-1;

	}while(remain_len >0);
}