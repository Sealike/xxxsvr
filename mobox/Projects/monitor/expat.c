#include "expat.h"
#include "client.h"


void  exp_set_callback(_exp_t exp,pfunsearchexp_cb pfunc)
{
	exp->m_cb  = pfunc;
}

void  exp_process_data(_exp_t exp,char* data,int len)
{
	int   curr_pos = 0;
	char* str = data;
	int   remain_len = len;
	sess_t S = NULL;
	do{
		int status;

		if (strcmp(str, "") == 0)
		{
			str = str + 1;
			remain_len = remain_len - 1;
			continue;
		}

		status = exp->search(exp,str,remain_len,&curr_pos);		

		if(status==PACK_UNSEARCHED || status==PACK_UNFINISHED)
			break;
		
		if (exp->explain(exp, exp->frame, exp->framelen) == -1)
		{
			int i = 0;
			i++;
			break;
		}

		str = str + curr_pos+1;
		remain_len = remain_len - curr_pos-1;

	}while(remain_len >0);
}