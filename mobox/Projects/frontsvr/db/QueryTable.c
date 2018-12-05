#include "StdAfx.h"
#include "QueryTable.h"

void CQueryTable::cond_query(mysqlquery_t handle,char* condition,void* out)
{
	if(handle == NULL) return ;
	
	char strsql[1024];
	if(condition)
		sprintf_s(strsql,sizeof(strsql),"SELECT %s FROM %s WHERE %s",m_columns,m_tblname,condition);
	else
		sprintf_s(strsql,sizeof(strsql),"SELECT %s FROM %s",m_columns,m_tblname);

	int rt = mysql_real_query(handle->m_con,strsql,strlen(strsql));

	if(rt == 0){
		handle->m_res = mysql_store_result(handle->m_con);
		while(handle->m_row = mysql_fetch_row(handle->m_res)) fill(handle->m_row,out);			
		mysql_free_result(handle->m_res);
	}else{
		TRACE1("query error : %s",mysql_error(handle->m_con));
	}
}

int CQueryTable::cond_delete(mysqlquery_t handle,char* condition)
{
	int rtn = 0;

	if(handle != NULL){
		char strsql[1024];
		if(condition)
			sprintf_s(strsql,sizeof(strsql),"DELETE FROM %s WHERE %s",m_tblname,condition);
		else
			sprintf_s(strsql,sizeof(strsql),"DELETE FROM %s",m_tblname);

		int rt = mysql_real_query(handle->m_con,strsql,strlen(strsql));
		if(rt == 0)
		{
			rtn = mysql_affected_rows(handle->m_con);
		}else{
			printf("delete query error :%s, %s, %s",m_tblname,strsql,mysql_error(handle->m_con));
		}
	}

	return rtn;
}

int  CQueryTable::cond_insert(mysqlquery_t handle,char* columns,char* values)
{
	int rtn = 0;
	
	if(handle != NULL){	
		char strsql[1024*8];
		sprintf_s(strsql,sizeof(strsql),"INSERT INTO %s(%s) VALUES (%s)",m_tblname,columns,values);

		int rt = mysql_real_query(handle->m_con,strsql,strlen(strsql));
		if(rt == 0)
		{
			rtn = mysql_affected_rows(handle->m_con);
		}else{
			TRACE1("insert query error :%s\n",mysql_error(handle->m_con));
		}
	}

	return rtn;
}

int  CQueryTable::cond_update(mysqlquery_t handle,char* setstring,char* condition)
{
	int rtn = 0;
	if(handle != NULL){
		char strsql[1024];
		if(condition)
			sprintf_s(strsql,sizeof(strsql),"UPDATE %s SET %s WHERE %s",m_tblname,setstring,condition);
		else
			sprintf_s(strsql,sizeof(strsql),"UPDATE %s SET %s",m_tblname,setstring);

		int rt = mysql_real_query(handle->m_con,strsql,strlen(strsql));
		if(rt == 0){
			rtn = mysql_affected_rows(handle->m_con);
		}else{
			TRACE1("update query error %s\n",mysql_error(handle->m_con));
		}
	}

	return rtn;
}