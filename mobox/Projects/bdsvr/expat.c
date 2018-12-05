#include "expat.h"



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
		int exprslt = 0;

		if (strcmp(str, "") == 0)
		{
			str = str + 1;
			remain_len = remain_len - 1;
			continue;
		}

		status = exp->search(exp,str,remain_len,&curr_pos);		

		if(status==PACK_UNSEARCHED || status==PACK_UNFINISHED)
			break;
		
		exprslt = exp->explain(exp, exp->frame, exp->framelen);
		if (exprslt == -1) break;

		str = str + curr_pos+1;
		remain_len = remain_len - curr_pos-1;

	}while(remain_len >0);
}