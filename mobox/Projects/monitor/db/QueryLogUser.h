#pragma once
#include "QueryTable.h"
#include "messagess.h"
#include <vector>
using namespace std;

template<typename TBasequery>
class CQueryLogUser : public TBasequery
{
public:
	CQueryLogUser():TBasequery("tbLogUser","username,psw"){;}

	~CQueryLogUser(void){;}

protected:
	virtual void fill(MYSQL_ROW row,void* out){
		loguser_st msg;
		char* val = NULL;
		vector<loguser_st>* pcont = (vector<loguser_st>*)out;

		val = row[0];
		if(val) 
			msg.username = val;

		val = row[1];
		if(val)	msg.pwd = val;
		pcont->push_back(msg);
	}
};
