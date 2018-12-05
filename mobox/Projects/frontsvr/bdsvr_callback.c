#include "bdsvr_callback.h"
#include "p2p.h"
#include "protobuffer.h"
#include "bdsvr_expat.h"
#include "json/cJSON.h"
#include "client.h"
#include "tool.h"
#include "dbaccess.h"
#include "forward.h"
#include "getrefsvr.h"
#include "userstatus.h"


extern int g_warntime;
//static sess_t getrefsvr(const char* cname)
//{
//	if (client()->sessions == NULL) return NULL;
//	sess_t rtn = (sess_t)xhash_get(client()->sessions, cname);
//
//	return rtn;
//}

static char * float2str(float val, int precision, char *buf)
{
	char *cur, *end;

	sprintf(buf, "%.6f", val);
	if (precision < 6) {
		cur = buf + strlen(buf) - 1;
		end = cur - 6 + precision;
		while ((cur > end) && (*cur == '0')) {
			*cur = '\0';
			cur--;
		}
	}

	return buf;
}


int translate_to_degree(char * jstr,char*out)
{
	// lon = 3803.3969

	char val[20] = { 0 };

	printf("before translate: %s.\n",jstr);
	// to float = 3803.3969f;
	float flt = atof(jstr);

	// 小数点左移两位 -> 38.033969
	flt = flt / 100.0;

	// 取出38 和 0.033969
	int integer = flt;
	float decimal = flt - integer;

	// 制作出03.3969
	decimal = decimal * 100.0;

	// rslt = 38 + 03.3969/60.0;
	float rslt = integer + decimal / 60.0;

	sprintf(val,"%.7f", rslt);
	// to char*
	//if (NULL == float2str(rslt, 7, val))
	//{
	//	return -1;
	//}

	memcpy(out, val, sizeof(val));

	printf("after translate: %s.\n", out);
	return 0;
}

static void auth_hook(char* jstr,sess_t sess)
{
	// 首先验证身份，回复auth信息给bdsvr/app
	cJSON *jsonroot, *json;
	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL)
		return;
	char *pwd, *name, *seqno, *taskid, *loc;

	json = cJSON_GetObjectItem(jsonroot, "taskid");
	if (json == NULL || json->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}
	taskid = json->valuestring;

	snprintf(sess->sname, sizeof(sess->sname), "%s", taskid);

	// 如果有旧连接，则将旧连接断开
	xht ht = client()->sessions;
	sess_t oldsess = NULL;
	oldsess = xhash_get(ht, sess->sname);
	if (oldsess != NULL)
	{
		// 待完善
		printf("kill oldsess %s\n", oldsess->sname);
		p2p_kill((p2p_t)oldsess);
	}

	json = cJSON_GetObjectItem(jsonroot, "name");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	name = json->valuestring;

	json = cJSON_GetObjectItem(jsonroot, "seqno");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	seqno = json->valuestring;

	json = cJSON_GetObjectItem(jsonroot, "pwd");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	pwd = json->valuestring;

	


	// 对于app用户，需要验证身份，如果验证通过，还需要再发送logon日志给monitor
	if (strcmp(name, "bdsvr01") != 0)
	{
		// 查数据库，核对人员身份
		char * realpwd = NULL;
		db_auth_excecutor((mysqlquery_t)client()->sqlobj, name, taskid, &realpwd);

		// 验证失败，回复auth fail
		if (realpwd == NULL || strcmp(pwd, realpwd) != 0)
		{
			int cnt = 0;
			char buffer[2048] = { 0 };
			p2p_t p2p = (p2p_t)sess;
			response_pkt_p pkt = NULL;
			unsigned char ucheck;
			cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$auth,%s,%s,{\"rslt\":\"err\",\"error\":\"密码错误\",\"seqno\":\"%s\"}", "frontsvr", sess->sname, seqno);
			ucheck = checksum((unsigned char*)buffer, cnt);
			cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
			pkt = response_pkt_new(client()->pktpool, cnt);
			memcpy(pkt->data, buffer, cnt);
			pkt->len = cnt;
			pkt->data[pkt->len] = '\0';
			p2p_stream_push(p2p, pkt);
			free(realpwd);

			log_write(p2p->sess.client->log, LOG_NOTICE, "send auth fail ack to %s:%s.", sess->sname, buffer);
			p2p_kill((p2p_t)sess);
			cJSON_Delete(jsonroot);
			return;
		}
		else
		{
			// 验证通过，回复auth ok
			int cnt = 0;
			char buffer[2048] = { 0 };
			p2p_t p2p = (p2p_t)sess;
			response_pkt_p pkt = NULL;
			unsigned char ucheck;

			SYSTEMTIME t;
			GetLocalTime(&t);
			char strtime[64] = { 0 };

			sprintf_s(strtime, sizeof(strtime), "%04d-%02d-%02d %02d:%02d:%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

			cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$auth,%s,%s,{\"rslt\":\"ok\",\"seqno\":\"%s\",\"time\":\"%s\"}", "frontsvr", sess->sname, seqno, strtime);
			ucheck = checksum((unsigned char*)buffer, cnt);
			cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
			pkt = response_pkt_new(client()->pktpool, cnt);
			memcpy(pkt->data, buffer, cnt);
			pkt->len = cnt;
			pkt->data[pkt->len] = '\0';
			p2p_stream_push(p2p, pkt);
			free(realpwd);
			xhash_put(client()->sessions, sess->sname, sess);

			log_write(p2p->sess.client->log, LOG_NOTICE, "send auth ack to %s:%s.", sess->sname, buffer);


			// 如果验证通过，对于互联网链路的验证，需要转发login的log信息给monitor，并入库
			json = cJSON_GetObjectItem(jsonroot, "loc");
			if (json == NULL) {
				cJSON_Delete(jsonroot);
				return;
			}
			cJSON* item = cJSON_GetObjectItem(json, "lon");
			if (item == NULL || item->valuestring == NULL)
			{
				cJSON_Delete(jsonroot);
				return -1;
			}
			char *lon = item->valuestring;

			item = cJSON_GetObjectItem(json, "lat");
			if (item == NULL || item->valuestring == NULL)
			{
				cJSON_Delete(jsonroot);
				return -1;
			}
			char *lat = item->valuestring;

			item = cJSON_GetObjectItem(json, "hgt");
			if (item == NULL || item->valuestring == NULL)
			{
				cJSON_Delete(jsonroot);
				return -1;
			}
			char *hgt = item->valuestring;
			
			memset(buffer, 0, sizeof(buffer));
			int count = 0;
			int endpos = 0;

			count += sprintf_s(buffer + count, sizeof(buffer) - count, "$log,%s,%s,{\"taskid\":\"%s\",\"executor\":\"%s\",\"net\":\"%s\",\"data\":{\"time\":\"%s\",\"type\":\"%s\"},\"loc\":{\"lon\":\"%s\",\"lat\":\"%s\",\"hgt\":\"%s\"}}", "frontsvr", "monitor", taskid, name, "inet", strtime, "logon", lon, lat, hgt);
			count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
			endpos = count;
			buffer[endpos] = '\0';

			// 先入库
			plog_t plog = (plog_t)malloc(sizeof(plog_st));
			plog->net = "inet";
			plog->executor = name;
			plog->taskid = atoi(taskid);
			plog->source = "frontsvr";
			plog->time = strtime;
			plog->type = "logon";
			plog->lon = lon;
			plog->lat = lat;
			plog->hgt = hgt;
			plog->desc = "";

			db_save_plog((mysqlquery_t)client()->sqlobj, plog);
			free(plog);

			sess_t destsess;
			char *monitor = "monitor";
			destsess = getrefsvr(monitor);

			if (destsess == NULL)
			{
				cJSON_Delete(jsonroot);
				return;
			}

			p2p = (p2p_t)destsess;
			pkt = response_pkt_new(client()->pktpool, count);
			memcpy(pkt->data, buffer, count);
			pkt->len = count;
			p2p_stream_push(p2p, pkt);

			log_write(p2p->sess.client->log, LOG_NOTICE, "send log[logon] to monitor:%s.", buffer);


			// 在线状态更新
			if (client()->user_act)
			{
				user_status_arg_t us_arg = (user_status_arg_t)malloc(sizeof(user_status_arg_st));
				strcpy_s(us_arg->executor, strlen(name) + 1, name);
				strcpy_s(us_arg->lon, strlen(lon) + 1, lon);
				strcpy_s(us_arg->lat, strlen(lat) + 1, lat);
				strcpy_s(us_arg->hgt, strlen(hgt) + 1, hgt);
				users_act_save(client()->user_act, taskid, _max_life, _user_inet, us_arg);
				free(us_arg);
				log_write(client()->log, LOG_NOTICE, "users_act_save in auth_hook. key=[%s]", taskid);
			}
		}
	}
	else
	{
		// 针对bdsvr，验证通过，回复auth ok
		int cnt = 0;
		char buffer[2048] = { 0 };
		p2p_t p2p = (p2p_t)sess;
		response_pkt_p pkt = NULL;
		unsigned char ucheck;
		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$auth,%s,%s{\"rslt\":\"ok\",\"seqno\":\"%s\"}", "frontsvr", sess->sname, seqno);
		ucheck = checksum((unsigned char*)buffer, cnt);
		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
		pkt = response_pkt_new(client()->pktpool, cnt);
		memcpy(pkt->data, buffer, cnt);
		pkt->len = cnt;
		pkt->data[pkt->len] = '\0';
		p2p_stream_push(p2p, pkt);
		xhash_put(client()->sessions, sess->sname, sess);

		log_write(p2p->sess.client->log, LOG_NOTICE, "send auth ack to %s:%s.", sess->sname, buffer);
	}

	cJSON_Delete(jsonroot);
}



static void awake_hook(char* jstr, sess_t sess)
{
	char * awake = NULL;
	char buffer[2048] = { 0 };

	// 首先验证身份，回复auth信息给bdsvr/app
	cJSON *jsonroot, *json;
	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL)
		return;
	char *pwd, *name, *seqno, *taskid, *loc;

	json = cJSON_GetObjectItem(jsonroot, "name");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	name = json->valuestring;

	json = cJSON_GetObjectItem(jsonroot, "seqno");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	seqno = json->valuestring;

	json = cJSON_GetObjectItem(jsonroot, "pwd");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	pwd = json->valuestring;

	json = cJSON_GetObjectItem(jsonroot, "loc");
	if (json == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	cJSON* item = cJSON_GetObjectItem(json, "lon");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}
	char *lon = item->valuestring;

	item = cJSON_GetObjectItem(json, "lat");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}
	char *lat = item->valuestring;

	item = cJSON_GetObjectItem(json, "hgt");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}
	char *hgt = item->valuestring;

	json = cJSON_GetObjectItem(jsonroot, "taskid");
	if (json == NULL || json->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}
	taskid = json->valuestring;


	// 验证身份，然后发送awake日志给monitor，无论成败
	char * realpwd = NULL;
	db_auth_excecutor((mysqlquery_t)client()->sqlobj, name,taskid, &realpwd);

	SYSTEMTIME t;
	GetLocalTime(&t);
	char strtime[64] = { 0 };

	sprintf_s(strtime, sizeof(strtime), "%04d-%02d-%02d %02d:%02d:%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

	// 验证失败，回复awake fail给箱子
	if (realpwd == NULL || strcmp(pwd, realpwd) != 0)
	{
		int cnt = 0;
		memset(buffer, 0, sizeof(buffer));
		p2p_t p2p = (p2p_t)sess;
		response_pkt_p pkt = NULL;
		unsigned char ucheck;
		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$awakeack,%s,%s,{\"rslt\":\"err\",\"error\":\"密码错误\",\"seqno\":\"%s\"}", "frontsvr", sess->sname, seqno);
		ucheck = checksum((unsigned char*)buffer, cnt);
		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
		pkt = response_pkt_new(client()->pktpool, cnt);
		memcpy(pkt->data, buffer, cnt);
		pkt->len = cnt;
		pkt->data[pkt->len] = '\0';
		p2p_stream_push(p2p, pkt);
		free(realpwd);

		log_write(p2p->sess.client->log, LOG_NOTICE, "send awake fail ack to %s:%s.", sess->sname, buffer);

		awake = "awakefail";
	}
	else
	{
		// 验证通过，回复awake ok给箱子，然后更新sess
		int cnt = 0;
		memset(buffer, 0, sizeof(buffer));
		sess_rename(sess, taskid,sizeof(taskid));
		
		p2p_t p2p = (p2p_t)sess;
		response_pkt_p pkt = NULL;
		unsigned char ucheck;

		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$awakeack,%s,%s,{\"rslt\":\"ok\",\"seqno\":\"%s\",\"time\":\"%s\"}", "frontsvr", sess->sname, seqno, strtime);
		ucheck = checksum((unsigned char*)buffer, cnt);
		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
		pkt = response_pkt_new(client()->pktpool, cnt);
		memcpy(pkt->data, buffer, cnt);
		pkt->len = cnt;
		pkt->data[pkt->len] = '\0';
		p2p_stream_push(p2p, pkt);
		free(realpwd);

		log_write(p2p->sess.client->log, LOG_NOTICE, "send awake ack to %s:%s.", sess->sname, buffer);
	
		awake = "awakeok";
	}

	memset(buffer, 0, sizeof(buffer));
	int count = 0;
	int endpos = 0;

	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$log,%s,%s,{\"taskid\":\"%s\",\"executor\":\"%s\",\"net\":\"%s\",\"data\":{\"time\":\"%s\",\"type\":\"%s\"},\"loc\":{\"lon\":\"%s\",\"lat\":\"%s\",\"hgt\":\"%s\"}}", "frontsvr", "monitor", taskid, name, "inet", strtime, awake, lon, lat, hgt);
	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
	endpos = count;
	buffer[endpos] = '\0';


	// 将日志入库
	plog_t plog = (plog_t)malloc(sizeof(plog_st));
	plog->net = "inet";
	plog->executor = name;
	plog->taskid = atoi(taskid);
	plog->source = "frontsvr";
	plog->time = strtime;
	plog->type = awake;
	plog->lon = lon;
	plog->lat = lat;
	plog->hgt = hgt;
	plog->desc = "";

	db_save_plog((mysqlquery_t)client()->sqlobj, plog);
	free(plog);

	// 将日志转发给monitor
	sess_t destsess;
	char *monitor = "monitor";
	destsess = getrefsvr(monitor);

	if (destsess == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}

	p2p_t p2p = (p2p_t)destsess;
	response_pkt_p pkt = response_pkt_new(client()->pktpool, count);
	memcpy(pkt->data, buffer, count);
	pkt->len = count;
	p2p_stream_push(p2p, pkt);

	log_write(p2p->sess.client->log, LOG_NOTICE, "send log[%s] to monitor:%s.", buffer,awake);

	// 在线状态更新
	if (client()->user_act)
	{
		user_status_arg_t us_arg = (user_status_arg_t)malloc(sizeof(user_status_arg_st));
		strcpy_s(us_arg->executor, strlen(name) + 1, name);
		strcpy_s(us_arg->lon, strlen(lon) + 1, lon);
		strcpy_s(us_arg->lat, strlen(lat) + 1, lat);
		strcpy_s(us_arg->hgt, strlen(hgt) + 1, hgt);
		users_act_save(client()->user_act, taskid, _max_life, _user_inet, us_arg);
		free(us_arg);
		log_write(client()->log, LOG_NOTICE, "users_act_save in awake_hook. key=[%s]", taskid);
	}

	cJSON_Delete(jsonroot);
}

// 将来自箱子的原生日志进行解析，然后分字段存储，入库
static void save_orig_log(char *jstr)
{
	//log_write(client()->log, LOG_NOTICE, "save_orig_log:%s.",  jstr);

	p2p_t p2p;
	sess_t destsess;

	cJSON *jsonroot, *json;
	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL)
		return;
	char *taskid, *executor, *net;

	json = cJSON_GetObjectItem(jsonroot, "taskid");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	taskid = json->valuestring;


	json = cJSON_GetObjectItem(jsonroot, "executor");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	executor = json->valuestring;


	json = cJSON_GetObjectItem(jsonroot, "net");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	net = json->valuestring;

	//////////////////////////////////////////////////////////////////////////
	// 解析data
	json = cJSON_GetObjectItem(jsonroot, "data");
	if (json == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}

	cJSON* item = cJSON_GetObjectItem(json, "type");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return -1;
	}
	char *type = item->valuestring;

	//////////////////////////////////////////////////////////////////////////
	//解析loc
	json = cJSON_GetObjectItem(jsonroot, "loc");
	if (json == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}

	item = cJSON_GetObjectItem(json, "lon");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return -1;
	}
	char *lon = item->valuestring;

	item = cJSON_GetObjectItem(json, "lat");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return -1;
	}
	char *lat = item->valuestring;

	item = cJSON_GetObjectItem(json, "hgt");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return -1;
	}
	char *hgt = item->valuestring;


	SYSTEMTIME t;
	GetLocalTime(&t);
	char strtime[64] = { 0 };

	sprintf_s(strtime, sizeof(strtime), "%04d-%02d-%02d %02d:%02d:%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

	log_write(client()->log, LOG_NOTICE, "save_orig_log: save [%s] log:%s\n", type, jstr);


	plog_t plog = (plog_t)malloc(sizeof(plog_st));
	plog->net = net;
	plog->executor = executor;
	plog->taskid = atoi(taskid);
	plog->source = "frontsvr";
	plog->time = strtime;
	plog->type = type;
	plog->lon = lon;
	plog->lat = lat;
	plog->hgt = hgt;
	plog->desc = "";

	db_save_plog(client()->sqlobj, plog);
	free(plog);

	// 在线状态更新
	if (client()->user_act)
	{
		user_status_arg_t us_arg = (user_status_arg_t)malloc(sizeof(user_status_arg_st));
		strcpy_s(us_arg->executor, strlen(executor) + 1, executor);
		strcpy_s(us_arg->lon, strlen(lon) + 1, lon);
		strcpy_s(us_arg->lat, strlen(lat) + 1, lat);
		strcpy_s(us_arg->hgt, strlen(hgt) + 1, hgt);
		users_act_save(client()->user_act, taskid, _max_life, _user_inet, us_arg);
		free(us_arg);
		log_write(client()->log, LOG_NOTICE, "users_act_save in save_orig_log. key=[%s]", taskid);
	}

	cJSON_Delete(jsonroot);
}


// 将来自箱子的设备状态解析，然后分字段存储，入库
static void save_status(char *jstr)
{
	cJSON *jsonroot, *json;
	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL)
		return;
	char *taskid, *executor, *net;

	json = cJSON_GetObjectItem(jsonroot, "taskid");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	taskid = json->valuestring;


	json = cJSON_GetObjectItem(jsonroot, "executor");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	executor = json->valuestring;


	json = cJSON_GetObjectItem(jsonroot, "net");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	net = json->valuestring;

	//////////////////////////////////////////////////////////////////////////
	// 解析data
	json = cJSON_GetObjectItem(jsonroot, "data");
	if (json == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}

	cJSON* item = cJSON_GetObjectItem(json, "battery");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return -1;
	}
	char *battery = item->valuestring;

	item = cJSON_GetObjectItem(json, "beam");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return -1;
	}
	char *beam = item->valuestring;

	//////////////////////////////////////////////////////////////////////////
	//解析loc
	json = cJSON_GetObjectItem(jsonroot, "loc");
	if (json == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}

	item = cJSON_GetObjectItem(json, "lon");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return -1;
	}
	char *lon = item->valuestring;

	item = cJSON_GetObjectItem(json, "lat");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return -1;
	}
	char *lat = item->valuestring;

	item = cJSON_GetObjectItem(json, "hgt");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return -1;
	}
	char *hgt = item->valuestring;


	SYSTEMTIME t;
	GetLocalTime(&t);
	char strtime[64] = { 0 };

	sprintf_s(strtime, sizeof(strtime), "%04d-%02d-%02d %02d:%02d:%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

	log_write(client()->log, LOG_NOTICE, "save_status:%s", jstr);


	status_t status = (plog_t)malloc(sizeof(plog_st));
	status->net = net;
	status->executor = executor;
	status->taskid = atoi(taskid);
	status->source = "frontsvr";
	status->time = strtime;
	status->battery = battery;
	status->beam = beam;
	status->lon = lon;
	status->lat = lat;
	status->hgt = hgt;
	

	db_save_status(client()->sqlobj, status);
	free(status);

	// 在线状态更新
	if (client()->user_act)
	{
		user_status_arg_t us_arg = (user_status_arg_t)malloc(sizeof(user_status_arg_st));
		strcpy_s(us_arg->executor, strlen(executor) + 1, executor);
		strcpy_s(us_arg->lon, strlen(lon) + 1, lon);
		strcpy_s(us_arg->lat, strlen(lat) + 1, lat);
		strcpy_s(us_arg->hgt, strlen(hgt) + 1, hgt);
		users_act_save(client()->user_act, taskid, _max_life, _user_inet, us_arg);
		free(us_arg);
		log_write(client()->log, LOG_NOTICE, "users_act_save in save_status. key=[%s]", taskid);
	}

	cJSON_Delete(jsonroot);
}



static void save_x_log(char *type, char *jstr)
{
	p2p_t p2p;
	sess_t destsess;


	log_write(client()->log, LOG_NOTICE, "save_x_log1: %s:%s.", type, jstr);


	cJSON *jsonroot, *json;
	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL)
		return;
	char *taskid, *executor, *net;

	json = cJSON_GetObjectItem(jsonroot, "taskid");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	taskid = json->valuestring;

	//log_write(client()->log, LOG_NOTICE, "save_x_log2.");
	json = cJSON_GetObjectItem(jsonroot, "executor");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	executor = json->valuestring;

	//log_write(client()->log, LOG_NOTICE, "save_x_log3.");
	json = cJSON_GetObjectItem(jsonroot, "net");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	net = json->valuestring;

	//log_write(client()->log, LOG_NOTICE, "save_x_log4.");

	//////////////////////////////////////////////////////////////////////////
	// 解析data
	json = cJSON_GetObjectItem(jsonroot, "data");
	if (json == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}

	//log_write(client()->log, LOG_NOTICE, "save_x_log5.");

	cJSON* item = cJSON_GetObjectItem(json, "desc");
	/*if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return -1;
	}*/
	char *desc = "";
	if (item != NULL && item->valuestring != NULL)
	{
		char *desc = item->valuestring;
	}
	


	//log_write(client()->log, LOG_NOTICE, "save_x_log6.");
	//////////////////////////////////////////////////////////////////////////
	//解析loc
	json = cJSON_GetObjectItem(jsonroot, "loc");
	if (json == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}


	//log_write(client()->log, LOG_NOTICE, "save_x_log6.");

	item = cJSON_GetObjectItem(json, "lon");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return -1;
	}
	char *lon = item->valuestring;

	//log_write(client()->log, LOG_NOTICE, "save_x_log7.");

	item = cJSON_GetObjectItem(json, "lat");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return -1;
	}
	char *lat = item->valuestring;

	//log_write(client()->log, LOG_NOTICE, "save_x_log8.");

	item = cJSON_GetObjectItem(json, "hgt");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return -1;
	}
	char *hgt = item->valuestring;

	//log_write(client()->log, LOG_NOTICE, "save_x_log9.");
	SYSTEMTIME t;
	GetLocalTime(&t);
	char strtime[64] = { 0 };

	sprintf_s(strtime, sizeof(strtime), "%04d-%02d-%02d %02d:%02d:%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

	//log_write(client()->log, LOG_NOTICE, "save_x_log10.");
	//log_write(client()->log, LOG_NOTICE, "save_x_log: save [%s] log:%s\n", type, jstr);


	plog_t plog = (plog_t)malloc(sizeof(plog_st));
	plog->net = net;
	plog->executor = executor;
	plog->taskid = atoi(taskid);
	plog->source = "frontsvr";
	plog->time = strtime;
	plog->type = type;
	plog->lon = lon;
	plog->lat = lat;
	plog->hgt = hgt;
	plog->desc = desc;

	db_save_plog(client()->sqlobj, plog);
	free(plog);

	// 在线状态更新
	if (client()->user_act)
	{
		user_status_arg_t us_arg = (user_status_arg_t)malloc(sizeof(user_status_arg_st));
		strcpy_s(us_arg->executor, strlen(executor) + 1, executor);
		strcpy_s(us_arg->lon, strlen(lon) + 1, lon);
		strcpy_s(us_arg->lat, strlen(lat) + 1, lat);
		strcpy_s(us_arg->hgt, strlen(hgt) + 1, hgt);
		users_act_save(client()->user_act, taskid, _max_life, _user_inet, us_arg);
		free(us_arg);
		log_write(client()->log, LOG_NOTICE, "users_act_save in save_x_log. key=[%s]", taskid);
	}

	cJSON_Delete(jsonroot);
}

// 将来自箱子的位置信息（箱子通过4G直接发来，或者通过北斗发给bdsvr，再由bdsvr处理后发来）解析分段入库
static save_position(char *jstr)
{

	//log_write(client()->log, LOG_NOTICE, "save_position1：%s.",jstr);
	cJSON *jsonroot, *json;
	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL)
		return;
	char *taskid, *executor, *net;

	json = cJSON_GetObjectItem(jsonroot, "taskid");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	taskid = json->valuestring;

	//log_write(client()->log, LOG_NOTICE, "save_position2.");

	json = cJSON_GetObjectItem(jsonroot, "executor");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	executor = json->valuestring;

	//log_write(client()->log, LOG_NOTICE, "save_position3.");
	json = cJSON_GetObjectItem(jsonroot, "net");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	net = json->valuestring;

	//////////////////////////////////////////////////////////////////////////
	// 解析data
	json = cJSON_GetObjectItem(jsonroot, "data");
	if (json == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}

	cJSON* item = cJSON_GetObjectItem(json, "lon");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return -1;
	}
	char *lon = item->valuestring;

	item = cJSON_GetObjectItem(json, "lat");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return -1;
	}
	char *lat = item->valuestring;

	item = cJSON_GetObjectItem(json, "hgt");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return -1;
	}
	char *hgt = item->valuestring;

	SYSTEMTIME t;
	GetLocalTime(&t);
	char strtime[64] = { 0 };

	sprintf_s(strtime, sizeof(strtime), "%04d-%02d-%02d %02d:%02d:%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

	

	log_write(client()->log, LOG_NOTICE, "save_boxpos: %s", jstr);


	boxpos_t boxpos = (boxpos_t)malloc(sizeof(boxpos_st));
	
	boxpos->net = net;
	boxpos->executor = executor;
	boxpos->taskid = atoi(taskid);
	boxpos->source = "frontsvr";
	boxpos->time = strtime;
	boxpos->lon = lon;
	boxpos->lat = lat;
	boxpos->hgt = hgt;


	db_save_boxpos(client()->sqlobj, boxpos);
	free(boxpos);

	// 在线状态更新
	if (client()->user_act)
	{
		user_status_arg_t us_arg = (user_status_arg_t)malloc(sizeof(user_status_arg_st));
		strcpy_s(us_arg->executor, strlen(executor) + 1, executor);
		strcpy_s(us_arg->lon, strlen(lon) + 1, lon);
		strcpy_s(us_arg->lat, strlen(lat) + 1, lat);
		strcpy_s(us_arg->hgt, strlen(hgt) + 1, hgt);
		users_act_save(client()->user_act, taskid, _max_life, _user_inet, us_arg);
		free(us_arg);
		log_write(client()->log, LOG_NOTICE, "users_act_save in save_position. key=[%s]",taskid);
	}

	cJSON_Delete(jsonroot);

}

// 将来自箱子的告警信息（箱子通过4G直接发来，或者通过北斗发给bdsvr，再由bdsvr处理后发来）解析分段入库
static save_warning(char *jstr)
{

	log_write(client()->log, LOG_NOTICE, "save_warning: %s", jstr);

	cJSON *jsonroot, *json;
	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL)
		return;
	char *taskid, *executor, *net;

	json = cJSON_GetObjectItem(jsonroot, "taskid");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	taskid = json->valuestring;


	json = cJSON_GetObjectItem(jsonroot, "executor");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	executor = json->valuestring;


	json = cJSON_GetObjectItem(jsonroot, "net");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	net = json->valuestring;

	//////////////////////////////////////////////////////////////////////////
	// 解析data
	json = cJSON_GetObjectItem(jsonroot, "data");
	if (json == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}

	cJSON* item = cJSON_GetObjectItem(json, "type");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return -1;
	}
	char *type = item->valuestring;


	json = cJSON_GetObjectItem(jsonroot, "loc");
	if (json == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	item = cJSON_GetObjectItem(json, "lon");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return -1;
	}
	char *lon = item->valuestring;

	item = cJSON_GetObjectItem(json, "lat");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return -1;
	}
	char *lat = item->valuestring;

	item = cJSON_GetObjectItem(json, "hgt");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return -1;
	}
	char *hgt = item->valuestring;


	SYSTEMTIME t;
	GetLocalTime(&t);
	char strtime[64] = { 0 };

	sprintf_s(strtime, sizeof(strtime), "%04d-%02d-%02d %02d:%02d:%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);



	log_write(client()->log, LOG_NOTICE, "save_warning: %s", jstr);


	warning_t warning = (warning_t)malloc(sizeof(warning_st));

	warning->net = net;
	warning->executor = executor;
	warning->taskid = atoi(taskid);
	warning->source = "frontsvr";
	warning->time = strtime;
	warning->lon = lon;
	warning->lat = lat;
	warning->hgt = hgt;
	warning->type = type;


	db_save_warning(client()->sqlobj, warning);
	free(warning);

	// 在线状态更新
	if (client()->user_act)
	{
		user_status_arg_t us_arg = (user_status_arg_t)malloc(sizeof(user_status_arg_st));
		strcpy_s(us_arg->executor, strlen(executor) + 1, executor);
		strcpy_s(us_arg->lon, strlen(lon) + 1, lon);
		strcpy_s(us_arg->lat, strlen(lat) + 1, lat);
		strcpy_s(us_arg->hgt, strlen(hgt) + 1, hgt);
		users_act_save(client()->user_act, taskid, _max_life, _user_inet, us_arg);
		free(us_arg);
		log_write(client()->log, LOG_NOTICE, "users_act_save in save_warning. key=[%s]", taskid);
	}

	cJSON_Delete(jsonroot);
}


//收到bdsvr发送的数据，转发给warnsvr/monitor
void  bdsvr_exp_cb(int msgid ,void* msg,int len,void* param)
{
	sess_t sess,destsess;
	p2p_t p2p;
	int  msgcode = 0;
	unsigned char* frame = (unsigned char*)msg;
	bdsvr_exp_t exp = (bdsvr_exp_t)param;
	
	sess = (sess_t)_BASE(exp).param;
	p2p = (p2p_t)sess;
	if (exp == NULL || sess == NULL) return;

	char * jstr = NULL;

	if (len - 5 < 0){
		p2p_kill(p2p);
		return;
	}

	jstr = (char*)frame;
	jstr[len - 5] = '\0';
	while (*jstr != '{')jstr++;

	switch(msgid)
	{
	case cunpack_ping:
		{
			log_write(client()->log, LOG_NOTICE, "recv ping from %s.", sess->sname);
			protobuffer_send_p2p(p2p, eProto_pong, sess->sname);
		}
		break;

	case cunpack_auth:
		{
			log_write(client()->log, LOG_NOTICE, "recv auth %s.", frame);
			auth_hook(jstr,sess);
		}
		break;

	case cunpack_awake:
	{
		log_write(client()->log, LOG_NOTICE, "recv awake from bdsvr/app:%s.", (char *)frame);
		awake_hook(jstr, sess);
	}
		break;

	case cunpack_position:
		{
			log_write(client()->log, LOG_NOTICE, "recv pos from bdsvr/app:%s.", (char *)frame);
			save_position(jstr);
			forward_to_warnsvr("pos", jstr);
			forward_to_monitor("pos", jstr);
		}
		break;

	case cunpack_warning:
		{
			log_write(client()->log, LOG_NOTICE, "recv warning from bdsvr/app:%s.", (char *)frame);
			save_warning(jstr);
			forward_to_monitor("warn", jstr);
		}
		break;

	case cunpack_log:
		{
			log_write(client()->log, LOG_NOTICE, "recv log from bdsvr/app:%s.", (char *)frame);
			forward_to_monitor("log", jstr);		// 来自箱子的原生日志，需要直接存库
			save_orig_log(jstr);					// 存库
		}
		break;

	case cunpack_status:
	{
		log_write(client()->log, LOG_NOTICE, "recv status from bdsvr/app:%s.", (char *)frame);
		forward_to_monitor("sta", jstr);
		save_status(jstr);
	}
	break;

	case cunpack_reqcode:
		{
			log_write(client()->log, LOG_NOTICE, "++++ recv reqcode from bdsvr/app:%s. ++++", (char *)frame);

			forward_to_warnsvr("reqcode", jstr);
			forward_log_x_to_monitor("reqcode", jstr);
		}
		break;

	case cunpack_taskpau:
		{
			log_write(client()->log, LOG_NOTICE, "recv taskpau from bdsvr/app:%s.", (char *)frame);	

			forward_to_monitor("taskpau", jstr);
			forward_log_x_to_monitor("taskpau", jstr);	
		}
		break;

	case cunpack_taskcon:
		{
			log_write(client()->log, LOG_NOTICE, "recv taskcon from bdsvr/app:%s.", (char *)frame);

			forward_to_monitor("taskcon", jstr);
			forward_log_x_to_monitor("taskcon", jstr);   
		}
		break;

	case cunpack_taskdone:
	{
		log_write(client()->log, LOG_NOTICE, "recv taskdone from bdsvr/app:%s.", (char *)frame);

		forward_to_monitor("taskdone", jstr);
		forward_log_x_to_monitor("taskdone", jstr);
	}
	break;

	default:
		break;
	}
	
}
