#pragma once
#include "QueryTable.h"
#include "messagess.h"
#include <vector>
using namespace std;

template<typename TBasequery>
class CQueryAccount : public TBasequery
{
public:
	CQueryAccount():TBasequery("tbAccountInfo","username,pwd,name,workunit,age,mail"){;}
	~CQueryAccount(void){;}

protected:
	virtual void fill(MYSQL_ROW row,void* out){
		account_st msg;
		char* val = NULL;
		vector<account_st>* pcont = (vector<account_st>*)out;
		memset(&msg,0,sizeof(msg));
		val = row[0];
		if(val) 
			msg.username = val;

		val = row[1];
		if(val)	msg.pwd = val;

		val = row[2];
		if(val)
			msg.strname = val;

		val = row[3];
		if(val)
			msg.strworkunit = val;
		
		val = row[4];
		if(val)
			msg.iage = atoi(val);

		val = row[5];
		if(val)
			msg.strmail = val;

		pcont->push_back(msg);
	}
};
