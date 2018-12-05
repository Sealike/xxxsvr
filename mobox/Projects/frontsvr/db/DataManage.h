#pragma once

class CDataManage
{
public:
	CDataManage(void);
	~CDataManage(void);
};

#define _DECLARE_INIT_QUERYTABLE() protected: void InitQuerytables();

#define _ADD_QUERYTABLE(queryclass) protected: queryclass m_xx##queryclass; \
	public: queryclass* Get##queryclass(){return &m_xx##queryclass;}

#define _INIT_QUERYTABLE(queryclass) m_xx##queryclass.set_dbresource(this);
#define _INIT_TABLE_BEGIN(queryclass) void queryclass::InitQuerytables(){
#define _INIT_TABLE_END() }

struct IDBQuery{
	virtual BOOL db_connect(const char* strip,int port,const char* dbuser,const char* pwd,const char* dbname,BOOL ifcontinue_ping,int pingseconds){return FALSE;}
	virtual BOOL db_close(){return FALSE;}
	virtual BOOL startping(int seconds){return FALSE;}
	virtual void stopping(){;}
};

class CConditionQuery{
public:

	virtual void cond_query(char* condition,void* out){out = NULL;}
	virtual int  cond_delete(char* condition){return 0;}
	virtual int  cond_insert(char* columns,char* values){return 0;}
	virtual int  cond_update(char* setstring,char* condition){return 0;}
	
};

