#pragma once
#include "QueryTable.h"
#include "messagess.h"
#include <vector>
using namespace std;

template<typename TBasequery>
class CQueryScore : public TBasequery
{
public:
	CQueryScore():TBasequery("tbScore","orderdate,session,username,nickame,age,workunit,mail,ordernumber,score"){;}
	~CQueryScore(void){;}

protected:
	virtual void fill(MYSQL_ROW row,void* out){
		userscore_st msg;
		char* val = NULL;
		vector<userscore_st>* pcont = (vector<userscore_st>*)out;

		memset(&msg,0,sizeof(msg));

		val = row[0];
		if(val) 
			msg.strdate = val;

		val = row[1];
		if(val)	msg.isession = atoi(val);

		val = row[2];
		if(val) 
			msg.strusername = val;

		val = row[3];
		if(val) 
			msg.strname = val;

		val = row[4];
		if(val) 
			msg.iage=atoi( val);


		val = row[5];
		if(val) 
			msg.strworkunit = val;


		val = row[6];
		if(val) 
			msg.strmail = val;


		val = row[7];
		if(val) 
			msg.strordernumber = val;

		val = row[8];
		if(val) 
			msg.score = val;

		pcont->push_back(msg);
	}
};
