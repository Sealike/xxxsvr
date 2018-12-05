#pragma once
#include "mysqldb.h"
#include "ChnavtectGlobal.h"
#include "DataManage.h"
#include "type.h"

class CQueryTable :public CConditionQuery
{
public:
	CQueryTable(char* tblname,char* column){
		m_tblname = _strdup(tblname);
		m_columns = _strdup(column); 
	}

	~CQueryTable(void){
		if(m_tblname) free((void*)m_tblname); 
		if(m_columns) free((void*)m_columns);
	}

	virtual void set_dbresource(CSharedRes* res){
		m_dbres = res;
	}

	virtual void cond_query(char* condition,void* out){
		AUTOLOCK((*m_dbres->get_reslock()));
		cond_query((mysqlquery_t)m_dbres->get_queryhandle(),condition,out);
	}

	virtual int  cond_delete(char* condition){
		AUTOLOCK((*m_dbres->get_reslock())); 
		return cond_delete((mysqlquery_t)m_dbres->get_queryhandle(),condition);
	}

	virtual int  cond_insert(char* columns,char* values){
		AUTOLOCK((*m_dbres->get_reslock())); 
		return cond_insert((mysqlquery_t)m_dbres->get_queryhandle(),columns,values);
	}

	virtual int  cond_update(char* setstring,char* condition){
		AUTOLOCK((*m_dbres->get_reslock())); 
		return cond_update((mysqlquery_t)m_dbres->get_queryhandle(),setstring,condition);
	}
protected:
	void cond_query(mysqlquery_t handle,char* condition,void* out);
	int  cond_delete(mysqlquery_t handle,char* condition);
	int  cond_insert(mysqlquery_t handle,char* columns,char* values);
	int  cond_update(mysqlquery_t handle,char* setstring,char* condition);
	virtual void fill(MYSQL_ROW row,void* out){;}
private:
	const char* m_columns;
	const char* m_tblname;
	CSharedRes* m_dbres;
};

