#include "expat.h"
#include "client.h"


void   exp_clear(_exp_t exp)
{
	exp->framelen = 0;
	exp->searchstatus = SEARCHMACHINE_NULL;
}

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
			// opensess.c中，要进行exp的释放，此时如果再继续执行后面的search等操作，就会引起异常，故需要break
			break;
		}

		str = str + curr_pos+1;
		remain_len = remain_len - curr_pos-1;

	}while(remain_len >0);
}