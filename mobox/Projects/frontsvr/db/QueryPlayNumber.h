#pragma once
#include "QueryTable.h"
#include "messagess.h"
#include <vector>
using namespace std;

template<typename TBasequery>
class CQueryPlayNumber : public TBasequery
{
public:
	CQueryPlayNumber():TBasequery("tbPlayNumber","id,ordernumber,maketime"){;}
	~CQueryPlayNumber(void){;}

protected:
	virtual void fill(MYSQL_ROW row,void* out){
		playnumber_st msg;
		char* val = NULL;
		vector<playnumber_st>* pcont = (vector<playnumber_st>*)out;
		val = row[0];
		if(val) 
			msg.id = atoi(val);

		val = row[1];
		if(val)	
			msg.strordernumber = val;

		val = row[2];
		if(val)	
			msg.strmaketime = val;
		pcont->push_back(msg);
	}
};
