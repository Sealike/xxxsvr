#include "dbaccess.h"
#include "CirQueue.h"
#include "util/util.h"
#include "instapp.h"

extern instapp_t g_instapp;

// 根据北斗号，查询设备号和任务号与执行人代号
int db_get_hid_taskid_executor_by_bdid(mysqlquery_t dbinst, char * bdid, char **taskid, char **hid, char **executor)
{
	char sql[512] = { 0 };
	int  rt = -1;
	int  q = -1;

	if (dbinst == NULL || hid < 0 || taskid == NULL || executor == NULL)
		return -1;

	sprintf(sql, "SELECT t.id taskid,b.devid hid, p.code executor FROM t_task t JOIN t_box b ON t.boxid=b.id JOIN t_person p ON t.executeid=p.id WHERE b.bdid=%s", bdid);
	q = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (q == 0)
	{
		dbinst->m_res = mysql_store_result(dbinst->m_con);
		while (dbinst->m_row = mysql_fetch_row(dbinst->m_res))
		{
			if (dbinst->m_row[0] && dbinst->m_row[1] && dbinst->m_row[2])
			{
				*taskid = _strdup(dbinst->m_row[0]);
				*hid = _strdup(dbinst->m_row[1]);
				*executor = _strdup(dbinst->m_row[2]);
				rt = 0;
				break;
			}	
		}
		mysql_free_result(dbinst->m_res);
	}
	else
	{
		log_write(g_instapp->log, LOG_ERR, "db_get_hid_taskid_executor_by_bdid . query error :%s", mysql_error(dbinst->m_con));
	}

	return rt;
}




// 根据主机号hid，获取任务号taskid
// t_task中
int db_get_taskid_byhid(mysqlquery_t dbinst,int hid, char **taskid)
{
	char sql[512] = { 0 };
	int  rt = 0;

	if (dbinst == NULL || hid < 0 || taskid == NULL)
		return -1;

	sprintf(sql, "SELECT id FROM t_task WHERE boxid=%d", hid);
	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0)
	{
		dbinst->m_res = mysql_store_result(dbinst->m_con);
		while (dbinst->m_row = mysql_fetch_row(dbinst->m_res))
		{
			if (dbinst->m_row[0])
			{
				*taskid = _strdup(dbinst->m_row[0]);
				rt = 0;
				break;
			}
			else
				rt = -1;
		}
		mysql_free_result(dbinst->m_res);
	}
	else{
		log_debug(ZONE, "db_get_taskid_byhid . query error :%s", mysql_error(dbinst->m_con));
		rt = -1;
	}

	return rt;
}

// 根据北斗号，获取主机号hid
// t_box中，id为int类型，即hid
int db_get_hid_bybdid(mysqlquery_t dbinst, char * bdid, char **hid)
{
	char sql[512] = { 0 };
	int  rt = 0;

	if (dbinst == NULL || bdid == NULL || hid == NULL)
		return -1;

	sprintf(sql, "SELECT devid FROM t_box WHERE bdid=%s", bdid);
	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0)
	{
		dbinst->m_res = mysql_store_result(dbinst->m_con);
		while (dbinst->m_row = mysql_fetch_row(dbinst->m_res))
		{
			if (dbinst->m_row[0])
			{
				*hid = _strdup(dbinst->m_row[0]);
				break;
			}
			else
				rt = -1;		
		}
		mysql_free_result(dbinst->m_res);
	}
	else{
		log_debug(ZONE, "db_get_hid_bybdid . query error :%s", mysql_error(dbinst->m_con));
		rt = -1;
	}

	return rt;
}


// 根据组ID获取组员ID
int db_get_groupusers(mysqlquery_t dbinst, int igid, Rslt_GroupUsers_t Q)
{
	char sql[512] = { 0 };
	int  rt = 0;
	if (dbinst == NULL || igid < 0)
		return rt;

	sprintf(sql, "SELECT * FROM t_group_user WHERE group_id=%d", igid);
	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0)
	{
		dbinst->m_res = mysql_store_result(dbinst->m_con);
		while (dbinst->m_row = mysql_fetch_row(dbinst->m_res))
		{
			if (dbinst->m_row[2])
			{
				EnQueue(Q, atoi(dbinst->m_row[2]));
			}else
				break;
		}
		mysql_free_result(dbinst->m_res);
	}
	else{
		log_debug(ZONE, "db_get_groupusers . error :%s", mysql_error(dbinst->m_con));
	}

	return 1;
}

int db_load_groupusers(mysqlquery_t dbinst, int ipid/*平台id*/,int gid, groupinfos_t rslt)
{
	char sql[512] = { 0 };
	int  rt = 1;
	int printn = 0;
	if (dbinst == NULL || gid <0 || rslt == NULL)
		return rt;

	groupinfos_clear(rslt);

	printn += sprintf(sql + printn, "SELECT *  FROM t_group_user ", gid);

	if (gid > 0)
		printn += sprintf(sql + printn, "WHERE id=%d  AND group_id=%d", ipid,gid);

	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0)
	{
		dbinst->m_res = mysql_store_result(dbinst->m_con);
		while (dbinst->m_row = mysql_fetch_row(dbinst->m_res))
		{
			userinfo_t info = userinfo_new();

			if (dbinst->m_row[2])
				info->id = atoi(dbinst->m_row[2]);
			groupinfos_add(rslt, info);
		}
		mysql_free_result(dbinst->m_res);
	}
	else{
		if (dbinst->_cb)
			dbinst->_cb(dbinst, eSqlQueryerr_errorping);

		log_debug(ZONE, "db_load_group . query error :%s", mysql_error(dbinst->m_con));
	}
	return rt;
}

int db_load_sos(mysqlquery_t dbinst, int ipid, sos_t rslt)		// 下载sos
{
	char sql[512] = { 0 };
	int  rt = 1;
	int printn = 0;
	if (dbinst == NULL || ipid < 0 || rslt == NULL)
		return rt;

	sos_clear(rslt);

	printn += sprintf(sql + printn, "SELECT *  FROM t_user_friend ", ipid);

	if (ipid > 0)
		printn += sprintf(sql + printn, "WHERE id=%d", ipid);

	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0)
	{
		dbinst->m_res = mysql_store_result(dbinst->m_con);
		while (dbinst->m_row = mysql_fetch_row(dbinst->m_res))
		{
			userinfo_t info = userinfo_new();

			if (dbinst->m_row[4] == "yes")
				info->id = atoi(dbinst->m_row[1]);
			sos_add(rslt, info);
		}
		mysql_free_result(dbinst->m_res);
	}
	else{
		if (dbinst->_cb)
			dbinst->_cb(dbinst, eSqlQueryerr_errorping);

		log_debug(ZONE, "db_load_sos . query error :%s", mysql_error(dbinst->m_con));
	}
	return rt;
}

int db_load_posreport(mysqlquery_t dbinst, int ipid, pos_t rslt)
{
	char sql[512] = { 0 };
	int  rt = 1;
	int printn = 0;
	if (dbinst == NULL || ipid < 0 || rslt == NULL)
		return rt;

	pos_clear(rslt);

	printn += sprintf(sql + printn, "SELECT *  FROM t_user_friend ", ipid);

	if (ipid > 0)
		printn += sprintf(sql + printn, "WHERE id=%d", ipid);

	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0)
	{
		dbinst->m_res = mysql_store_result(dbinst->m_con);
		while (dbinst->m_row = mysql_fetch_row(dbinst->m_res))
		{
			userinfo_t info = userinfo_new();

			if (dbinst->m_row[5] == "yes")
				info->id = atoi(dbinst->m_row[1]);
			pos_add(rslt, info);
		}
		mysql_free_result(dbinst->m_res);
	}
	else{
		if (dbinst->_cb)
			dbinst->_cb(dbinst, eSqlQueryerr_errorping);

		log_debug(ZONE, "db_load_posreport . query error :%s", mysql_error(dbinst->m_con));
	}
	return rt;
}

int db_query_bdid(mysqlquery_t dbinst, int ipid)
{
	char sql[512] = { 0 };
	int  rt = 0;
	int printn = 0;
	if (dbinst == NULL || ipid < 0 )
		return rt;
	printn += sprintf(sql + printn, "SELECT *  FROM t_user ", ipid);

	if (ipid > 0)
		printn += sprintf(sql + printn, "WHERE id=%d", ipid);

	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0)
	{
		dbinst->m_res = mysql_store_result(dbinst->m_con);
		while (dbinst->m_row = mysql_fetch_row(dbinst->m_res))
		{
			if (dbinst->m_row[5])
				rt = atoi(dbinst->m_row[5]);
			break;
		}
		mysql_free_result(dbinst->m_res);
	}
	else{
		if (dbinst->_cb)
			dbinst->_cb(dbinst, eSqlQueryerr_errorping);

		log_debug(ZONE, "db_query_bdid . query error :%s", mysql_error(dbinst->m_con));
	}
	return rt;
}

// 取得某个北斗id相关联的平台id
int db_query_pid(mysqlquery_t dbinst, int ibdid)
{
	char sql[512] = { 0 };
	int  rt = 0;
	int printn = 0;
	if (dbinst == NULL || ibdid < 0)
		return rt;
	printn += sprintf(sql + printn, "SELECT *  FROM t_user ", ibdid);

	if (ibdid > 0)
		printn += sprintf(sql + printn, "WHERE bdid=%d", ibdid);

	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0)
	{
		dbinst->m_res = mysql_store_result(dbinst->m_con);
		while (dbinst->m_row = mysql_fetch_row(dbinst->m_res))
		{
			if (dbinst->m_row[0])
				rt = atoi(dbinst->m_row[0]);
			break;
		}
		mysql_free_result(dbinst->m_res);
	}
	else{
		if (dbinst->_cb)
			dbinst->_cb(dbinst, eSqlQueryerr_errorping);

		log_debug(ZONE, "db_query_pid . query error :%s", mysql_error(dbinst->m_con));
	}
	return rt;
}

int db_query_phone(mysqlquery_t dbinst, int ipid, char **phonenumber)//取得某个平台相关联的手机号
{
	char sql[512] = { 0 };
	int  rt = 0;

	if (dbinst == NULL || ipid <0)
		return rt;

	sprintf(sql, "SELECT * FROM t_user WHERE id=%d", ipid);
	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0)
	{
		dbinst->m_res = mysql_store_result(dbinst->m_con);
		while (dbinst->m_row = mysql_fetch_row(dbinst->m_res))
		{
			if (dbinst->m_row[1])
				*phonenumber = _strdup(dbinst->m_row[1]);
			break;
		}
		mysql_free_result(dbinst->m_res);
	}
	else{
		log_debug(ZONE, "db_query_phone . query error :%s", mysql_error(dbinst->m_con));
	}

	return 1;
}
int db_query_mail(mysqlquery_t dbinst, int ipid, char **mailbox)//取得某个平台相关联的邮箱
{
	char sql[512] = { 0 };
	int  rt = 0;

	if (dbinst == NULL || ipid < 0)
		return rt;

	sprintf(sql, "SELECT * FROM t_user WHERE id=%d", ipid);
	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0)
	{
		dbinst->m_res = mysql_store_result(dbinst->m_con);
		while (dbinst->m_row = mysql_fetch_row(dbinst->m_res))
		{
			if (dbinst->m_row[3])
				*mailbox = _strdup(dbinst->m_row[3]);
			break;
		}
		mysql_free_result(dbinst->m_res);
	}
	else{
		log_debug(ZONE, "db_query_mail . query error :%s", mysql_error(dbinst->m_con));
	}

	return 1;
}

// 新增
int db_insert_sms2bdmsg(mysqlquery_t dbinst, sms2bdms_st info)//插入历史信息
{
	char sql[2048] = { 0 };
	int  rt = 0;
	int  affectedrows = 0;

	if (dbinst == NULL) return 0;
	sprintf(sql, "INSERT INTO tbsms2bdhistory(sendtime, codetype,sendphone ,rcvbid,content,delivertime,seqno,flg) VALUES('%s',%d,'%s',%d,'%s','%s',%d,%d)", info.sendtime, info.icodetype, info.sendphone, info.recvid, info.content, info.delivertime, info.seqno, info.iflag);
	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0){
		affectedrows = mysql_affected_rows(dbinst->m_con);
	}

	return affectedrows;
}

int db_insert_bd2smsmsg(mysqlquery_t dbinst, bd2sms_st info)
{
	char sql[2048] = { 0 };
	int  rt = 0;
	int  affectedrows = 0;

	if (dbinst == NULL) return 0;
	sprintf(sql, "INSERT INTO tbbd2smshistory(sendtime, codetype,sendbid ,rcvphone,content,delivertime,seqno,flg) VALUES('%s',%d,%d,'%s','%s','%s',%d,%d)", info.sendtime, info.icodetype, info.sendbid, info.recvphone, info.content, info.delivertime, info.seqno, info.iflag);
	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0){
		affectedrows = mysql_affected_rows(dbinst->m_con);
	}

	return affectedrows;
}

int db_update_sms2bdmsg(mysqlquery_t dbinst, char *sendtime, char* sendphone, char* delivertime, int iflg) //修改字段
{
	char sql[512];
	int  rt = 0;
	int  affectedrows = 0;
	if (dbinst == NULL || sendtime == NULL || delivertime == NULL)
		return 0;
	sprintf(sql, "UPDATE tbsms2bdhistory SET delivertime='%s',flg= %d WHERE sendtime='%s' AND  sendphone='%s'", delivertime, iflg, sendtime, sendphone);
	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));
	if (rt == 0){
		affectedrows = mysql_affected_rows(dbinst->m_con);
	}
	return affectedrows;

}

int db_update_bd2smsmsg(mysqlquery_t dbinst, char *sendtime, int sendbid, char* delivertime, int iflg)//修改字段
{
	char sql[512];
	int  rt = 0;
	int  affectedrows = 0;
	if (dbinst == NULL || sendtime == NULL || delivertime == NULL)
		return 0;
	sprintf(sql, "UPDATE tbbd2smshistory SET delivertime='%s',flg= %d WHERE sendtime='%s' AND  sendbid=%d", delivertime, iflg, sendtime, sendbid);
	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));
	if (rt == 0){
		affectedrows = mysql_affected_rows(dbinst->m_con);
	}
	return affectedrows;
}


int db_insert_smudpmsg(mysqlquery_t dbinst, sudp_message_st info)//插入历史信息
{
	char sql[2048] = { 0 };
	int  rt = 0;
	int  affectedrows = 0;

	if (dbinst == NULL) return 0;
	sprintf(sql, "INSERT INTO tbSmudpHistory(sendtime, codetype,sendid,rcvid,content,delivertime,seqno,flg) VALUES('%s',%d,%d,'%s','%s','%s',%d,%d)", info.sendtime, info.icomtype, info.isendid, info.recvid, info.content, info.delivertime, info.seqno, info.iflag);
	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0){
		affectedrows = mysql_affected_rows(dbinst->m_con);
	}

	return affectedrows;
}

int db_update_smudpmsg(mysqlquery_t dbinst, char *sendtime, int isendid, char* delivertime, int iflg)
{
	char sql[512];
	int  rt = 0;
	int  affectedrows = 0;
	if (dbinst == NULL || sendtime == NULL || delivertime == NULL)
		return 0;
	sprintf(sql, "UPDATE tbSmudpHistory SET delivertime='%s',flg= %d WHERE sendtime='%s' AND sendid = %d", delivertime, iflg, sendtime, isendid);
	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));
	if (rt == 0){
		affectedrows = mysql_affected_rows(dbinst->m_con);
	}
	return affectedrows;
}

int db_insert_bdsoshistory(mysqlquery_t dbinst, bdsos_st info)
{
	char sql[2048] = { 0 };
	int  rt = 0;
	int  affectedrows = 0;

	if (dbinst == NULL) return 0;
	sprintf(sql, "INSERT INTO tbbdsoshistory(postime, sendtime, sendbid, content,seqno) VALUES('%s','%s',%d,'%s',%d)", info.postime, info.sendtime, info.isendid, info.content, info.seqno);
	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0){
		affectedrows = mysql_affected_rows(dbinst->m_con);
	}

	return affectedrows;
}