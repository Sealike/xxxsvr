#include "dbaccess.h"

int db_save_log(mysqlquery_t dbinst,char * logstr)
{
	log_write(client()->log, LOG_NOTICE, "db_save_log.", logstr);
	
	char sql[512] = { 0 };
	int  rt = 0;
	int affectedrows = 0;
	int nprint = 0;
	int counter = 0;
	BOOL excuted = 0;
	int  sqlen[10] = { 0 };
	int i = 0;

	if (dbinst == NULL || logstr == NULL || dbinst->m_con == NULL)
	{
		log_write(client()->log, LOG_ERR, "db_save_log, error: query == NULL || query->m_con == NULL, return -1.", sql);
		return -1;
	}

	SYSTEMTIME t;
	GetLocalTime(&t);
	char strtime[64] = { 0 };

	sprintf_s(strtime, sizeof(strtime), "%04d-%02d-%02d %02d:%02d:%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

	sprintf(sql, "INSERT INTO t_plog(time,source,content) VALUES('%s','%s','%s')", strtime, "monitor", logstr);

	log_write(client()->log, LOG_NOTICE, "SQL = [%s].", sql);

	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0){
		affectedrows = mysql_affected_rows(dbinst->m_con);
		log_write(client()->log, LOG_NOTICE, "db_save_log. OK: affected rows = %d.", affectedrows);
	}
	else{
		log_write(client()->log, LOG_ERR, "db_save_log. error: %s.", mysql_error(dbinst->m_con));

		affectedrows = -1;
	}

	return affectedrows;
}



int db_loaduser(mysqlquery_t dbinst, char* userid, user_st * user)
{
	char sql[512] = { 0 };
	int  rt = 0;
	int printn = 0;

	if (dbinst == NULL || dbinst->m_con == NULL || userid == NULL || user == NULL)
	{
		log_write(client()->log, LOG_NOTICE, "db_loaduser. error: dbinst == NULL || dbinst->m_con == NULL.");
		return -1;
	}

	printn += sprintf(sql + printn, "SELECT name,towncode,countycode,countrycode,groupcode FROM t_user where userid='%s'", userid);
	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0)
	{
		dbinst->m_res = mysql_store_result(dbinst->m_con);
		while (dbinst->m_row = mysql_fetch_row(dbinst->m_res))
		{
			if (dbinst->m_row[0])
				strncpy(user->name, dbinst->m_row[0], sizeof(user->name)-1);
			if (dbinst->m_row[1])
				strncpy(user->towncode, dbinst->m_row[1], sizeof(user->towncode) - 1);
			if (dbinst->m_row[2])
				strncpy(user->countycode, dbinst->m_row[2], sizeof(user->countycode) - 1);
			if (dbinst->m_row[3])
				strncpy(user->countrycode, dbinst->m_row[3], sizeof(user->countrycode) - 1);
			if (dbinst->m_row[4])
				strncpy(user->groupcode, dbinst->m_row[4], sizeof(user->groupcode) - 1);
			rt = 0;
			break;
		}
		mysql_free_result(dbinst->m_res);
	}else{
		if (dbinst->_cb)
			dbinst->_cb(dbinst, eSqlQueryerr_errorping);

		rt = -1;
		printf("db_loaduser . query error :%s", mysql_error(dbinst->m_con));
	}

	return rt;

}

int db_loadbduser(mysqlquery_t dbinst, int bdid, char **userid)
{
	char sql[512] = { 0 };
	int  rt = 0;
	int printn = 0;

	if (dbinst == NULL || dbinst->m_con == NULL || bdid == 0 || userid == NULL)
	{
		log_write(client()->log, LOG_NOTICE, "db_loadbduser. error: dbinst == NULL || dbinst->m_con == NULL.");
		return -1;
	}

	printn += sprintf(sql + printn, "SELECT userid  FROM t_user where bdnum=%d", bdid);
	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0)
	{
		dbinst->m_res = mysql_store_result(dbinst->m_con);
		while (dbinst->m_row = mysql_fetch_row(dbinst->m_res))
		{
			if (dbinst->m_row[0]){
				*userid = _strdup(dbinst->m_row[0]);
				rt = 0;
			}				
		}
		mysql_free_result(dbinst->m_res);		
	}
	else{
		if (dbinst->_cb)
			dbinst->_cb(dbinst, eSqlQueryerr_errorping);

		rt = -1;
		printf("db_loadbduser . query error :%s", mysql_error(dbinst->m_con));
	}

	return rt;
}


int db_query_accountinfo(mysqlquery_t dbinst, char *accountname, account_t info)
{
	char sql[512] = { 0 };
	int  rt = 0;

	if (dbinst == NULL || dbinst->m_con  == NULL|| accountname == NULL || info == NULL)
	{
		log_write(client()->log, LOG_NOTICE, "db_query_accountinfo. error: dbinst == NULL || dbinst->m_con == NULL.");
		return -1;
	}

	sprintf(sql, "SELECT * FROM tbaccountinfo WHERE accountname='%s'", accountname);
	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0)
	{
		dbinst->m_res = mysql_store_result(dbinst->m_con);
		while (dbinst->m_row = mysql_fetch_row(dbinst->m_res))
		{
			if (dbinst->m_row[1])
				info ->pwd = _strdup(dbinst->m_row[1]);

			if (dbinst->m_row[2])
				info->itype = atoi(dbinst->m_row[2]);

			if (dbinst->m_row[3])
				info->code = _strdup(dbinst->m_row[3]);

			rt = 0;
			break;
		}
		mysql_free_result(dbinst->m_res);
	}
	else{
		rt = -1;
		printf("db_query_accountinfo . query error :%s", mysql_error(dbinst->m_con));
	}

	return rt;
}

int db_query_accountinfo_bycode(mysqlquery_t dbinst, char *code, account_t info)
{
	char sql[512] = { 0 };
	int  rt = 0;

	if (dbinst == NULL || dbinst->m_con == NULL || code == NULL || info == NULL)
	{
		log_write(client()->log, LOG_NOTICE, "db_query_accountinfo_bycode. error: dbinst == NULL || dbinst->m_con == NULL.");
		return -1;
	}

	sprintf(sql, "SELECT * FROM tbaccountinfo WHERE accountcode='%s'", code);
	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0)
	{
		dbinst->m_res = mysql_store_result(dbinst->m_con);
		while (dbinst->m_row = mysql_fetch_row(dbinst->m_res))
		{
			if (dbinst->m_row[0])
				info->name = _strdup(dbinst->m_row[0]);

			if (dbinst->m_row[1])
				info->pwd = _strdup(dbinst->m_row[1]);

			if (dbinst->m_row[2])
				info->itype = atoi(dbinst->m_row[2]);

			rt = 0;
			break;
		}
		mysql_free_result(dbinst->m_res);
	}
	else{
		rt = -1;
		printf("db_query_accountinfo_bycode . query error :%s", mysql_error(dbinst->m_con));
	}

	return rt;
}

int db_get_userid(mysqlquery_t dbinst, char* bdid, char **userid)
{
	char sql[512] = { 0 };
	int  rt = 0;
	int printn = 0;

	if (dbinst == NULL || dbinst->m_con == NULL || bdid == NULL || userid == NULL)
	{
		log_write(client()->log, LOG_ERR, "db_get_userid, error: query or query->m_con or bdid is NULL, return -1.");

		return rt = -1;
	}

	printn += sprintf(sql + printn, "SELECT userid  FROM t_user where bdnum=%s", bdid);
	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0)
	{
		dbinst->m_res = mysql_store_result(dbinst->m_con);
		while (dbinst->m_row = mysql_fetch_row(dbinst->m_res))
		{
			if (dbinst->m_row[0])
				*userid = _strdup(dbinst->m_row[0]);
			rt = 0;
		}
		mysql_free_result(dbinst->m_res);

	}
	else{
		if (dbinst->_cb)
			dbinst->_cb(dbinst, eSqlQueryerr_errorping);
		log_write(client()->log, LOG_NOTICE, "db_get_userid . query error :%s.", mysql_error(dbinst->m_con));
		rt = -1;
	}

	return rt;
}

int db_get_towncode(mysqlquery_t dbinst, char *userid, char ** towncode)
{
	char sql[512] = { 0 };
	int  rt = 0;
	int printn = 0;

	if (dbinst == NULL || dbinst->m_con == NULL || userid == NULL)
	{
		log_write(client()->log, LOG_ERR, "db_get_towncode, error: query or query->m_con is NULL, return -1.");

		return rt = -1;
	}

	printn += sprintf(sql + printn, "SELECT towncode  FROM t_user where userid=%s", userid);
	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0)
	{
		dbinst->m_res = mysql_store_result(dbinst->m_con);
		while (dbinst->m_row = mysql_fetch_row(dbinst->m_res))
		{
			if (dbinst->m_row[0])
				*towncode = _strdup(dbinst->m_row[0]);
			rt = 0;
		}
		mysql_free_result(dbinst->m_res);

	}
	else{
		if (dbinst->_cb)
			dbinst->_cb(dbinst, eSqlQueryerr_errorping);

		log_write(client()->log, LOG_NOTICE, "db_get_towncode . query error :%s.", mysql_error(dbinst->m_con));

		rt = -1;
	}

	return rt;
}

char * redis_get_towncode(char * userid)
{
	int rt = 0;
	char	black_key[64] = { 0 };
	char	userid_key[64] = { 0 };
	char	*towncode;

	if (userid == NULL)
	{
		log_write(client()->log, LOG_ERR, "redis_get_towncode: userid is NULL.");
		return NULL;
	}

	sprintf(black_key, "%s_black", userid);
	sprintf(userid_key, "%s_userid", userid);
	char * black = process_get_str(client()->redisUserinfo, black_key);	// �Ȳ������������redis�в��Ҽ�Ϊblack_key��ֵ
	if (black != NULL)     // ������������У���˵�����û����Ϸ�������Ҫ��������
	{
		free(black);
		black = NULL;
		process_push_str(client()->redisUserinfo, black_key, "NO");	    // ����ں������в��ҵ�������Ҫ���½�����ӵ���������
		set_activetime(client()->redisUserinfo, black_key, client()->redis_blife);
		return NULL;
	}
	else       // �����������û�У�����Ҫ����Ч���ݻ����в���
	{
		towncode = process_get_str(client()->redisUserinfo, userid_key);
		if (towncode == NULL)
		{
			// ����ڻ����в鲻��������Ҫ�������ݿ�
			rt = db_get_towncode((mysqlquery_t)client()->sqlobj, userid, &towncode);
			if (rt == -1 || userid == NULL)
			{
				// ��������ݿ��в���ʧ�ܣ�������ӵ���������
				process_push_str(client()->redisUserinfo, black_key, "NO");
				set_activetime(client()->redisUserinfo, black_key, client()->redis_blife);	// ���ú���������
				return NULL;
			}
			// ��������ݿ��в��ҳɹ���������ӵ���Ч�����У���ֱ�Ӷ�ȡ
			process_push_str(client()->redisUserinfo, userid_key, towncode);
			set_activetime(client()->redisUserinfo, userid_key, client()->redis_clife);		// ������Ч��������
			free(towncode);
			towncode = process_get_str(client()->redisUserinfo, userid_key);
		}
		// ����ڻ����в鵽����ֱ��ʹ�ã�����get������Ҫ���ڴ���malloc�����Ժ���ĸ�ֵ�Ͳ���Ҫ_strdup�ˣ�
	}

	return towncode;
}

char * redis_get_userid(char * bdid)
{
	int rt = 0;
	char	black_key[64] = { 0 };
	char	bdid_key[64] = { 0 };
	char	*userid;

	if (bdid == NULL)
	{
		log_write(client()->log, LOG_ERR, "redis_get_userid: bdid is NULL.");
		return NULL;
	}

	sprintf(black_key, "%s_black", bdid);
	sprintf(bdid_key, "%s_bdid", bdid);
	char * black = process_get_str(client()->redisUserinfo, black_key);	// �Ȳ������������redis�в��Ҽ�Ϊblack_key��ֵ
	if (black != NULL)     // ������������У���˵�����û����Ϸ�������Ҫ��������
	{
		free(black);
		black = NULL;
		process_push_str(client()->redisUserinfo, black_key, "NO");	    // ����ں������в��ҵ�������Ҫ���½�����ӵ���������
		set_activetime(client()->redisUserinfo, black_key, client()->redis_blife);
		return NULL;
	}
	else       // �����������û�У�����Ҫ����Ч���ݻ����в���
	{
		userid = process_get_str(client()->redisUserinfo, bdid_key);
		if (userid == NULL)
		{
			// ����ڻ����в鲻��������Ҫ�������ݿ�
			rt = db_get_userid((mysqlquery_t)client()->sqlobj, bdid, &userid);
			if (rt == -1 || userid == NULL)
			{
				// ��������ݿ��в���ʧ�ܣ�������ӵ���������
				process_push_str(client()->redisUserinfo, black_key, "NO");
				set_activetime(client()->redisUserinfo, black_key, client()->redis_blife);	// ���ú���������
				return NULL;
			}
			// ��������ݿ��в��ҳɹ���������ӵ���Ч�����У���ֱ�Ӷ�ȡ
			process_push_str(client()->redisUserinfo, bdid_key, userid);
			set_activetime(client()->redisUserinfo, bdid_key, client()->redis_clife);		// ������Ч��������
			free(userid);
			userid = process_get_str(client()->redisUserinfo, bdid_key);
		}
		// ����ڻ����в鵽����ֱ��ʹ�ã�����get������Ҫ���ڴ���malloc�����Ժ���ĸ�ֵ�Ͳ���Ҫ_strdup�ˣ�
	}

	return userid;
}

int db_save_event(mysqlquery_t dbinst, event_t evt)
{
	char sql[512] = { 0 };
	int  rt = 0;
	int affectedrows = 0;
	int nprint = 0;
	int counter = 0;
	BOOL excuted = 0;
	int  sqlen[10] = { 0 };
	int i = 0;

	if (dbinst == NULL || evt == NULL || dbinst->m_con == NULL)
	{
		log_write(client()->log, LOG_ERR, "db_save_event, error: query == NULL || query->m_con == NULL, return -1.", sql);
		return -1;
	}

	sprintf(sql, "INSERT INTO t_event(user_id, bd_id, towncode,time,lat,lon,hgt,type,`desc`,typename,stype) VALUES('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',%d)", evt->userid, evt->bdid, evt->towncode, evt->time, evt->lat, evt->lon, evt->hgt, evt->evttype, evt->description, evt->evttypname, evt->stype);

	log_write(client()->log, LOG_NOTICE, "SQL = [%s].", sql);

	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0){
		affectedrows = mysql_affected_rows(dbinst->m_con);

		affectedrows = mysql_affected_rows(dbinst->m_con);
		log_write(client()->log, LOG_NOTICE, "db_save_event. OK: affected rows = %d.", affectedrows);
	}
	else{
		log_write(client()->log, LOG_ERR, "db_save_event. error :%s.", mysql_error(dbinst->m_con));
		affectedrows = -1;
	}

	return affectedrows;
}

int db_save_position(mysqlquery_t dbinst, position_t position)
{
	char sql[512] = { 0 };
	int  rt = 0;
	int affectedrows = 0;
	int nprint = 0;
	int counter = 0;
	BOOL excuted = 0;
	int  sqlen[10] = { 0 };
	int i = 0;

	if (dbinst == NULL || position == NULL || dbinst->m_con == NULL)
	{
		log_write(client()->log, LOG_ERR, "db_save_position, error: query == NULL || query->m_con == NULL, return -1.", sql);
		return -1;
	}

	sprintf(sql, "INSERT INTO t_position(userid,bdid, towncode,time,lat,lon,hgt,stype) VALUES('%s','%s','%s','%s','%s','%s','%s',%d)", position->userid, position->bdid, position->towncode, position->time, position->lat, position->lon, position->hgt, position->stype);

	log_write(client()->log, LOG_NOTICE, "SQL = [%s].", sql);

	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0){
		affectedrows = mysql_affected_rows(dbinst->m_con);
		log_write(client()->log, LOG_NOTICE, "db_save_position. OK: affected rows = %d.", affectedrows);
	}
	else{
		log_write(client()->log, LOG_ERR, "db_save_position. error: %s.", mysql_error(dbinst->m_con));

		affectedrows = -1;
	}

	return affectedrows;
}

int _store_position(position_t pos)
{
	if (pos == NULL)
		return -1;

	mysqlquery_t sub_query = (mysqlquery_t)xhash_get(client()->xtowncode2subquery, pos->towncode);

	if ((sub_query != NULL) && (-1 != db_save_position(sub_query, pos)))
	{
		return 0;		// ������ݿ��д洢�ɹ�
	}

	else		// ������Ҳ��������ݿ����ӣ����߷����ݿ�洢ʧ�ܣ������������ݿ��д洢
	{
		mysqlquery_t main_query = (mysqlquery_t)client()->sqlobj;

		if (main_query == NULL || main_query->m_con == NULL)
		{
			return -1;	// �����ݿ�����ʧЧ���޷��洢
		}

		else if (-1 != db_save_position(main_query, pos))
		{
			return 0;	// �������ݿ��д洢�ɹ�
		}
		else
		{
			return -1;	// �������ݿ��д洢ʧ��
		}
	}
}

int _store_event(event_t evt)
{
	if (evt == NULL)
		return -1;

	mysqlquery_t sub_query = (mysqlquery_t)xhash_get(client()->xtowncode2subquery, evt->towncode);

	if ((sub_query != NULL) && (-1 != db_save_event(sub_query, evt)))
	{
		return -1;		// ������ݿ��д洢ʧ��
	}
	else		// ������Ҳ��������ݿ����ӣ����߷����ݿ�洢ʧ�ܣ������������ݿ��д洢
	{
		mysqlquery_t main_query = (mysqlquery_t)client()->sqlobj;

		if (main_query == NULL || main_query->m_con == NULL)
		{
			return -1;		// �����ݿ�����ʧЧ
		}

		else if (-1 != db_save_event(main_query, evt))
		{
			return 0;		// �������ݿ��д洢�ɹ�

		}
		else
		{
			return -1;		// �������ݿ��д洢ʧ��
		}
	}
}

static void dbconfig_free(dbconfig_t dbconfig)
{
	if (dbconfig == NULL)
		return;

	if (dbconfig->towncode)
		free(dbconfig->towncode);
	if (dbconfig->ip)
		free(dbconfig->ip);
	if (dbconfig->port)
		free(dbconfig->port);
	if (dbconfig->password)
		free(dbconfig->dbname);
	if (dbconfig->username)
		free(dbconfig->username);
	if (dbconfig->password)
		free(dbconfig->password);

	free(dbconfig);
}

int connect_each_database(mysqlquery_t dbinst)
{
	int i = 0;
	mysqlquery_t sub_query;

	char sql[512] = { 0 };
	int  rt = 0;

	if (dbinst == NULL || dbinst->m_con == NULL)
	{
		log_write(client()->log, LOG_ERR, "connect_each_database, error: query or query->m_con is NULL, return -1.");

		return rt = -1;
	}

	sprintf(sql, "SELECT * FROM t_dbconfig");
	rt = mysql_real_query(dbinst->m_con, sql, strlen(sql));

	if (rt == 0)
	{
		dbinst->m_res = mysql_store_result(dbinst->m_con);
		while (dbinst->m_row = mysql_fetch_row(dbinst->m_res))
		{
			if (strcmp(dbinst->m_row, ""))
			{
				dbconfig_t dbconfig = (dbconfig_t)malloc(sizeof(dbconfig_st));

				dbconfig->towncode = _strdup(dbinst->m_row[0]);
				dbconfig->ip = _strdup(dbinst->m_row[1]);
				dbconfig->port = _strdup(dbinst->m_row[2]);
				dbconfig->dbname = _strdup(dbinst->m_row[3]);
				dbconfig->username = _strdup(dbinst->m_row[4]);
				dbconfig->password = _strdup(dbinst->m_row[5]);


				// ���ÿһ��towncode������һ�����ݿ����ӣ�����towncode�����ݿ�������Ϊ��ֵ�ԣ������ϣ��g_instapp->sqlobjs��
				sub_query = mysqldb_connect_init(dbconfig->ip, atoi(dbconfig->port), dbconfig->username, dbconfig->password, dbconfig->dbname, NULL);
				if (sub_query == NULL)
				{
					log_write(client()->log, LOG_NOTICE, "sub_query connect to database [%s] failed. %d @%s\n", dbconfig->dbname, __LINE__, __FILE__);
					dbconfig_free(dbconfig);
					break;
				}
				else
				{
					mysql_query(sub_query->m_con, "SET NAMES 'gb2312'");
					log_write(client()->log, LOG_NOTICE, "sub_query connected db [%s] ok.", dbconfig->dbname);
				}

				xhash_put(client()->xdbconfig, dbconfig->towncode, dbconfig);		// ��towncodeΪ�����洢����������ݿ���������������Ϣ
				xhash_put(client()->xtowncode2subquery, dbconfig->towncode, sub_query);	// ��towncodeΪ�����洢��÷����ݿ������
				xhash_put(client()->xsubquery2towncode, sub_query, dbconfig->towncode); // ĳ�����ݿ������Ϊ�����洢�÷����ݿ��Ӧ��towncodeֵ
				jqueue_push(client()->sub_query_que, sub_query, 0);				// �������ӷ�������У�����mysql_ping����
			}
		}
		mysql_free_result(dbinst->m_res);
	}
	else{
		log_write(client()->log, LOG_NOTICE, "db_get_dbconfig . query error :%s.", mysql_error(dbinst->m_con));
		rt = -1;
	}

	return rt;
}
