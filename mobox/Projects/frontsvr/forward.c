#include "forward.h"
#include "dbaccess.h"
#include "getrefsvr.h"

extern int g_warntime;

void forward_to_app(char *head, char* jstr)
{
	p2p_t p2p;
	sess_t destsess;

	cJSON *jsonroot, *json;
	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL)
		return;
	char *net, *taskid;

	json = cJSON_GetObjectItem(jsonroot, "net");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	net = json->valuestring;

	// 如果网络类型是北斗，则直接转发给bdsvr
	if (strcmp("bd", net) == 0)
	{
		char *bdsvr = "bdsvr01";
		destsess = getrefsvr(bdsvr);
	}
	else
	{
		json = cJSON_GetObjectItem(jsonroot, "taskid");
		if (json == NULL || json->valuestring == NULL) {
			cJSON_Delete(jsonroot);
			return;
		}
		taskid = json->valuestring;
		destsess = getrefsvr(taskid);		// 如果是互联网类型，则根据用户名查找对应连接
	}

	if (destsess == NULL)
	{
		log_write(client()->log, LOG_NOTICE, "destsess = getrefsvr(%s) -> NULL.", taskid);
		cJSON_Delete(jsonroot);
		return;
	}

	int cnt = 0;
	char buffer[2048] = { 0 };
	response_pkt_p pkt = NULL;
	unsigned char ucheck;
	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$%s,%s,%s,%s", head, "frontsvr", destsess->sname, jstr);
	ucheck = checksum((unsigned char*)buffer, cnt);
	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
	//buffer[cnt] = '\0';
	p2p = (p2p_t)destsess;
	pkt = response_pkt_new(client()->pktpool, cnt);
	memcpy(pkt->data, buffer, cnt);
	pkt->len = cnt;
	pkt->data[pkt->len] = '\0';
	p2p_stream_push(p2p, pkt);

	log_write(client()->log, LOG_NOTICE, "++++ send %s to %s:%s. ++++", head, destsess->sname, buffer);
	cJSON_Delete(jsonroot);
}

void forward_to_warnsvr(char *head, char* jstr)
{
	char buffer[2048];
	int count = 0;
	int endpos = 0;
	response_pkt_p pkt;

	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$%s,%s,%s,%s", head, "frontsvr", "warnsvr", jstr);
	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
	endpos = count;
	buffer[endpos + 1] = '\0';

	if (client()->p2p_warnsvr != NULL)
	{
		pkt = response_pkt_new(((p2pclient_t)(client()->p2p_warnsvr))->pktpool, endpos + 1);
		memcpy(pkt->data, buffer, endpos + 1);
		pkt->len = endpos + 1;
		p2pclient_write((p2pclient_t)(client()->p2p_warnsvr), pkt);

		log_write(client()->log, LOG_NOTICE, "send %s ack to warnsvr:%s.", head, buffer);
	}
}

void forward_to_monitor(char *head, char *jstr)
{
	p2p_t p2p = NULL;
	sess_t destsess;

	char *monitor = "monitor";
	destsess = getrefsvr(monitor);
	if (destsess == NULL)
	{
		return;
	}

	int cnt = 0;
	char buffer[2048] = { 0 };
	response_pkt_p pkt = NULL;
	unsigned char ucheck;
	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$%s,%s,%s,%s", head, "frontsvr", "monitor", jstr);
	ucheck = checksum((unsigned char*)buffer, cnt);
	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
	buffer[cnt] = '\0';
	p2p = (p2p_t)destsess;
	pkt = response_pkt_new(client()->pktpool, cnt);
	memcpy(pkt->data, buffer, cnt);
	pkt->len = cnt;
	//pkt->data[pkt->len] = '\0';
	p2p_stream_push(p2p, pkt);

	log_write(p2p->sess.client->log, LOG_NOTICE, "send %s to monitor:%s.", head, buffer);
}

// 生成相应的日志，转发给monitor，并且存库
void forward_log_x_to_monitor(char *head, char *jstr)
{

	log_write(client()->log, LOG_NOTICE, "forward_log_x_to_monitor1:%s,%s.", head, jstr);
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

	SYSTEMTIME t;
	GetLocalTime(&t);
	char strtime[64] = { 0 };

	sprintf_s(strtime, sizeof(strtime), "%04d-%02d-%02d %02d:%02d:%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

	char buffer[2048];
	int count = 0;
	int endpos = 0;
	response_pkt_p pkt;

	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$log,%s,%s,{\"taskid\":\"%s\",\"executor\":\"%s\",\"net\":\"%s\",\"data\":{\"time\":\"%s\",\"type\":\"%s\"},\"loc\":{\"lon\":\"%s\",\"lat\":\"%s\",\"hgt\":\"%s\"}}", "frontsvr", "monitor", taskid, executor, net, strtime, head, lon, lat, hgt);
	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
	endpos = count;
	buffer[endpos] = '\0';

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
	//pkt->data[pkt->len] = '\0';
	p2p_stream_push(p2p, pkt);

	log_write(p2p->sess.client->log, LOG_NOTICE, "send %s log to monitor:%s\n", head, buffer);


	plog_t plog = (plog_t)malloc(sizeof(plog_st));
	plog->net = net;
	plog->executor = executor;
	plog->taskid = atoi(taskid);
	plog->source = "frontsvr";
	plog->time = strtime;
	plog->type = head;
	plog->lon = lon;
	plog->lat = lat;
	plog->hgt = hgt;
	plog->desc = "";

	log_write(client()->log, LOG_NOTICE, "forward_log_x_to_monitor10.");

	db_save_plog(client()->sqlobj, plog);
	free(plog);

	

	if (client()->user_act)
	{
		user_status_arg_t us_arg = (user_status_arg_t)malloc(sizeof(user_status_arg_st));
		strcpy_s(us_arg->executor, strlen(executor) + 1, executor);
		strcpy_s(us_arg->lon, strlen(lon) + 1, lon);
		strcpy_s(us_arg->lat, strlen(lat) + 1, lat);
		strcpy_s(us_arg->hgt, strlen(hgt) + 1, hgt);
		users_act_save(client()->user_act, taskid, _max_life, _user_inet, us_arg);
		free(us_arg);
		log_write(client()->log, LOG_NOTICE, "users_act_save in forward_log_x_to_monitor. key=[%s]", taskid);
	}

	cJSON_Delete(jsonroot);
}

void forward_log_dyncode_to_monitor(char *jstr)
{
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

	/*char *lon = "";
	char *lat = "";
	char *hgt = "";*/

	json = cJSON_GetObjectItem(jsonroot, "loc");
	if (json == NULL || json->valuestring == NULL) {
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

	char buffer[2048];
	int count = 0;
	int endpos = 0;
	response_pkt_p pkt;

	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$log,%s,%s,{\"taskid\":\"%s\",\"executor\":\"%s\",\"net\":\"%s\",\"data\":{\"time\":\"%s\",\"type\":\"%s\"},\"loc\":{\"lon\":\"%s\",\"lat\":\"%s\",\"hgt\":\"%s\"}}", "frontsvr", "monitor", taskid, executor, net, strtime, "dyncode", lon, lat, hgt);
	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
	endpos = count;
	buffer[endpos] = '\0';

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
	pkt->data[pkt->len] = '\0';
	p2p_stream_push(p2p, pkt);

	log_write(p2p->sess.client->log, LOG_NOTICE, "send dyncode log to monitor:%s\n", buffer);

	cJSON_Delete(jsonroot);
}

//void forward_dyncode_to_app(char * jstr)
//{
//	p2p_t p2p;
//	sess_t destsess;
//
//	cJSON *jsonroot, *json;
//	jsonroot = cJSON_Parse(jstr);
//	if (jsonroot == NULL)
//		return;
//	char *net, *executor;
//
//	json = cJSON_GetObjectItem(jsonroot, "net");
//	if (json == NULL || json->valuestring == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	net = json->valuestring;
//
//	 //如果网络类型是北斗，则直接转发给bdsvr
//	if (strcmp("bd", net) == 0)
//	{
//		char *bdsvr = "bdsvr01";
//		destsess = getrefsvr(bdsvr);
//	}
//	else
//	{
//		json = cJSON_GetObjectItem(jsonroot, "executor");
//		if (json == NULL || json->valuestring == NULL) {
//			cJSON_Delete(jsonroot);
//			return;
//		}
//		executor = json->valuestring;
//		destsess = getrefsvr(executor);		// 如果是互联网类型，则根据用户名查找对应连接
//	}
//
//	if (destsess == NULL)
//	{
//		cJSON_Delete(jsonroot);
//		return;
//	}
//
//	int cnt = 0;
//	char buffer[2048] = { 0 };
//	response_pkt_p pkt = NULL;
//	unsigned char ucheck;
//	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$dyncode,%s,%s,%s", "frontsvr", destsess->sname, jstr);
//	ucheck = checksum((unsigned char*)buffer, cnt);
//	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
//	buffer[cnt] = '\0';
//	p2p = (p2p_t)destsess;
//	pkt = response_pkt_new(client()->pktpool, cnt);
//	memcpy(pkt->data, buffer, cnt);
//	pkt->len = cnt;
//	pkt->data[pkt->len] = '\0';
//	p2p_stream_push(p2p, pkt);
//
//	log_write(client()->log, LOG_NOTICE, "++++ send dyncode to %s:%s. ++++", destsess->sname, buffer);
//	cJSON_Delete(jsonroot);
//}


void forward_warn_to_monitor(char * jstr)
{
	p2p_t p2p;
	sess_t destsess;

	char *monitor = "monitor";
	destsess = getrefsvr(monitor);
	if (destsess == NULL)
	{
		return;
	}

	int cnt = 0;
	char buffer[2048] = { 0 };
	response_pkt_p pkt = NULL;
	unsigned char ucheck;
	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$warn,%s,%s,%s", "frontsvr", "monitor", jstr);
	ucheck = checksum((unsigned char*)buffer, cnt);
	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
	buffer[cnt] = '\0';
	p2p = (p2p_t)destsess;
	pkt = response_pkt_new(client()->pktpool, cnt);
	memcpy(pkt->data, buffer, cnt);
	pkt->len = cnt;
	pkt->data[pkt->len] = '\0';
	p2p_stream_push(p2p, pkt);

	log_write(client()->log, LOG_NOTICE, "send warn to monitor:%s.", buffer);
}


 //把warn信息转发给app
void forward_warn_to_app(char *jstr)
{
	sess_t destsess;
	p2p_t p2p;
	cJSON *jsonroot, *json;
	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL)
		return;
	char *net, *executor;

	json = cJSON_GetObjectItem(jsonroot, "net");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	net = json->valuestring;

	 //如果网络类型是互联网，则转发给app，否则不转发
	if (strcmp("inet", net) == 0)
	{
		json = cJSON_GetObjectItem(jsonroot, "executor");
		if (json == NULL || json->valuestring == NULL) {
			cJSON_Delete(jsonroot);
			return;
		}
		executor = json->valuestring;
		destsess = getrefsvr(executor);		// 如果是互联网类型，则根据用户名查找对应连接

		if (destsess == NULL)
		{
			cJSON_Delete(jsonroot);
			return;
		}
		int cnt = 0;
		char buffer[2048] = { 0 };
		response_pkt_p pkt = NULL;
		unsigned char ucheck;
		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$warn,%s,%s,%s", "frontsvr", destsess->sname, jstr);
		ucheck = checksum((unsigned char*)buffer, cnt);
		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
		buffer[cnt] = '\0';
		p2p = (p2p_t)destsess;
		pkt = response_pkt_new(client()->pktpool, cnt);
		memcpy(pkt->data, buffer, cnt);
		pkt->len = cnt;
		pkt->data[pkt->len] = '\0';
		p2p_stream_push(p2p, pkt);

		log_write(client()->log, LOG_NOTICE, "send warn to %s:%s.", destsess->sname, buffer);
	}
	cJSON_Delete(jsonroot);
}


//
//void forward_reqcode_to_warnsvr(char* jstr)
//{
//	char buffer[2048];
//	int count = 0;
//	int endpos = 0;
//	response_pkt_p pkt;
//
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$reqcode,%s,%s,%s", "frontsvr", "warnsvr", jstr);
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
//	endpos = count;
//	buffer[endpos] = '\0';
//
//	if (client()->p2p_warnsvr != NULL)
//	{
//		pkt = response_pkt_new(((p2pclient_t)(client()->p2p_warnsvr))->pktpool, endpos + 1);
//		memcpy(pkt->data, buffer, endpos + 1);
//		pkt->len = endpos + 1;
//		p2pclient_write((p2pclient_t)(client()->p2p_warnsvr), pkt);
//
//		log_write(client()->log, LOG_NOTICE, "send reqcode to warnsvr:%s.", buffer);
//	}
//}
//
//void forward_log_reqcode_to_monitor(char *jstr)
//{
//	sess_t destsess;
//	p2p_t p2p;
//
//	char *monitor = "monitor";
//	destsess = getrefsvr(monitor);
//
//	if (destsess == NULL)
//	{
//		return;
//	}
//
//	cJSON *jsonroot, *json;
//	jsonroot = cJSON_Parse(jstr);
//	if (jsonroot == NULL)
//		return;
//	char *taskid, *executor, *net, *loc;
//
//	json = cJSON_GetObjectItem(jsonroot, "taskid");
//	if (json == NULL || json->valuestring == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	taskid = json->valuestring;
//
//	json = cJSON_GetObjectItem(jsonroot, "executor");
//	if (json == NULL || json->valuestring == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	executor = json->valuestring;
//
//	json = cJSON_GetObjectItem(jsonroot, "net");
//	if (json == NULL || json->valuestring == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	net = json->valuestring;
//
//	json = cJSON_GetObjectItem(jsonroot, "loc");
//	if (json == NULL || json->valuestring == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//
//	cJSON* item = cJSON_GetObjectItem(json, "lon");
//	if (item == NULL || item->valuestring == NULL)
//	{
//		cJSON_Delete(jsonroot);
//		return -1;
//	}
//	char *lon = item->valuestring;
//
//	item = cJSON_GetObjectItem(json, "lat");
//	if (item == NULL || item->valuestring == NULL)
//	{
//		cJSON_Delete(jsonroot);
//		return -1;
//	}
//	char *lat = item->valuestring;
//
//	item = cJSON_GetObjectItem(json, "hgt");
//	if (item == NULL || item->valuestring == NULL)
//	{
//		cJSON_Delete(jsonroot);
//		return -1;
//	}
//	char *hgt = item->valuestring;
//
//	SYSTEMTIME t;
//	GetLocalTime(&t);
//	char strtime[64] = { 0 };
//
//	sprintf_s(strtime, sizeof(strtime), "%04d-%02d-%02d %02d:%02d:%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
//
//	char buffer[2048];
//	int count = 0;
//	int endpos = 0;
//	response_pkt_p pkt;
//
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$log,%s,%s,{\"taskid\":\"%s\",\"executor\":\"%s\",\"net\":\"%s\",\"data\":{\"time\":\"%s\",\"type\":\"%s\"},\"loc\":{\"lon\":\"%s\",\"lat\":\"%s\",\"hgt\":\"%s\"}}", "frontsvr", "monitor", taskid, executor, net, strtime, "dyncode", lon, lat, hgt);
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
//	endpos = count;
//	buffer[endpos] = '\0';
//
//
//
//	p2p = (p2p_t)destsess;
//	pkt = response_pkt_new(client()->pktpool, count);
//	memcpy(pkt->data, buffer, count);
//	pkt->len = count;
//	//pkt->data[pkt->len] = '\0';
//	p2p_stream_push(p2p, pkt);
//
//	log_write(p2p->sess.client->log, LOG_NOTICE, "send log[reqcode] to monitor:%s.", buffer);
//
//	cJSON_Delete(jsonroot);
//}
//
//void forward_taskpau_to_monitor(char *jstr)
//{
//	p2p_t p2p = NULL;
//	sess_t destsess;
//
//	char *monitor = "monitor";
//	destsess = getrefsvr(monitor);
//
//	if (destsess == NULL)
//	{
//		return;
//	}
//
//	// 先把taskpau请求直接转发给monitor
//	int cnt = 0;
//	char buffer[2048] = { 0 };
//	response_pkt_p pkt = NULL;
//	unsigned char ucheck;
//	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$taskpau,%s,%s,%s", "frontsvr", "monitor", jstr);
//	ucheck = checksum((unsigned char*)buffer, cnt);
//	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
//	buffer[cnt] = '\0';
//	p2p = (p2p_t)destsess;
//	pkt = response_pkt_new(client()->pktpool, cnt);
//	memcpy(pkt->data, buffer, cnt);
//	pkt->len = cnt;
//	//pkt->data[pkt->len] = '\0';
//	p2p_stream_push(p2p, pkt);
//
//	log_write(p2p->sess.client->log, LOG_NOTICE, "send taskpau to monitor:%s.", buffer);
//}
//
//void forward_log_taskpau_to_monitor(char *jstr)
//{
//	p2p_t p2p = NULL;
//	sess_t destsess;
//
//	char *monitor = "monitor";
//	destsess = getrefsvr(monitor);
//
//	if (destsess == NULL)
//	{
//		return;
//	}
//
//
//	cJSON *jsonroot, *json;
//	jsonroot = cJSON_Parse(jstr);
//	if (jsonroot == NULL)
//		return;
//	char *taskid, *executor, *net;
//
//	json = cJSON_GetObjectItem(jsonroot, "taskid");
//	if (json == NULL || json->valuestring == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	taskid = json->valuestring;
//
//	json = cJSON_GetObjectItem(jsonroot, "executor");
//	if (json == NULL || json->valuestring == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	executor = json->valuestring;
//
//	json = cJSON_GetObjectItem(jsonroot, "net");
//	if (json == NULL || json->valuestring == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	net = json->valuestring;
//
//	json = cJSON_GetObjectItem(jsonroot, "loc");
//	if (json == NULL || json->valuestring == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//
//	cJSON* item = cJSON_GetObjectItem(json, "lon");
//	if (item == NULL || item->valuestring == NULL)
//	{
//		cJSON_Delete(jsonroot);
//		return -1;
//	}
//	char *lon = item->valuestring;
//
//	item = cJSON_GetObjectItem(json, "lat");
//	if (item == NULL || item->valuestring == NULL)
//	{
//		cJSON_Delete(jsonroot);
//		return -1;
//	}
//	char *lat = item->valuestring;
//
//	item = cJSON_GetObjectItem(json, "hgt");
//	if (item == NULL || item->valuestring == NULL)
//	{
//		cJSON_Delete(jsonroot);
//		return -1;
//	}
//	char *hgt = item->valuestring;
//
//	SYSTEMTIME t;
//	GetLocalTime(&t);
//	char strtime[64] = { 0 };
//
//	sprintf_s(strtime, sizeof(strtime), "%04d-%02d-%02d %02d:%02d:%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
//
//	char buffer[2048];
//	int count = 0;
//	int endpos = 0;
//	response_pkt_p pkt;
//
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$log,%s,%s,{\"taskid\":\"%s\",\"executor\":\"%s\",\"net\":\"%s\",\"data\":{\"time\":\"%s\",\"type\":\"%s\"},\"loc\":{\"lon\":\"%s\",\"lat\":\"%s\",\"hgt\":\"%s\"}}", "frontsvr", "monitor", taskid, executor, net, strtime, "taskpause", lon, lat, hgt);
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
//	endpos = count;
//	buffer[endpos] = '\0';
//
//	p2p = (p2p_t)destsess;
//	pkt = response_pkt_new(client()->pktpool, count);
//	memcpy(pkt->data, buffer, count);
//	pkt->len = count;
//	//pkt->data[pkt->len] = '\0';
//	p2p_stream_push(p2p, pkt);
//
//	log_write(p2p->sess.client->log, LOG_NOTICE, "send log[taskpause] log to monitor:%s.", buffer);
//	cJSON_Delete(jsonroot);
//}
//
//void forward_taskcon_to_monitor(char *jstr)
//{
//	p2p_t p2p = NULL;
//	sess_t destsess;
//
//	char *monitor = "monitor";
//	destsess = getrefsvr(monitor);
//
//	if (destsess == NULL)
//	{
//		return;
//	}
//
//	int cnt = 0;
//	char buffer[2048] = { 0 };
//	response_pkt_p pkt = NULL;
//	unsigned char ucheck;
//	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$taskcon,%s,%s,%s", "frontsvr", "monitor", jstr);
//	ucheck = checksum((unsigned char*)buffer, cnt);
//	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
//	buffer[cnt] = '\0';
//	p2p = (p2p_t)destsess;
//	pkt = response_pkt_new(client()->pktpool, cnt);
//	memcpy(pkt->data, buffer, cnt);
//	pkt->len = cnt;
//	//pkt->data[pkt->len] = '\0';
//	p2p_stream_push(p2p, pkt);
//
//	log_write(p2p->sess.client->log, LOG_NOTICE, "send taskcon to monitor:%s.", buffer);
//}
//
//void forward_log_taskcon_to_monitor(char *jstr)
//{
//	p2p_t p2p = NULL;
//	sess_t destsess;
//
//	char *monitor = "monitor";
//	destsess = getrefsvr(monitor);
//
//	if (destsess == NULL)
//	{
//		return;
//	}
//
//
//	cJSON *jsonroot, *json;
//	jsonroot = cJSON_Parse(jstr);
//	if (jsonroot == NULL)
//		return;
//	char *taskid, *executor, *net;
//
//	json = cJSON_GetObjectItem(jsonroot, "taskid");
//	if (json == NULL || json->valuestring == NULL) {
//		cJSON_Delete(jsonroot);
//		return 0;
//	}
//	taskid = json->valuestring;
//
//	json = cJSON_GetObjectItem(jsonroot, "executor");
//	if (json == NULL || json->valuestring == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	executor = json->valuestring;
//
//	json = cJSON_GetObjectItem(jsonroot, "net");
//	if (json == NULL || json->valuestring == NULL) {
//		cJSON_Delete(jsonroot);
//		return 0;
//	}
//	net = json->valuestring;
//
//	json = cJSON_GetObjectItem(jsonroot, "loc");
//	if (json == NULL || json->valuestring == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//
//	cJSON* item = cJSON_GetObjectItem(json, "lon");
//	if (item == NULL || item->valuestring == NULL)
//	{
//		cJSON_Delete(jsonroot);
//		return -1;
//	}
//	char *lon = item->valuestring;
//
//	item = cJSON_GetObjectItem(json, "lat");
//	if (item == NULL || item->valuestring == NULL)
//	{
//		cJSON_Delete(jsonroot);
//		return -1;
//	}
//	char *lat = item->valuestring;
//
//	item = cJSON_GetObjectItem(json, "hgt");
//	if (item == NULL || item->valuestring == NULL)
//	{
//		cJSON_Delete(jsonroot);
//		return -1;
//	}
//	char *hgt = item->valuestring;
//
//	SYSTEMTIME t;
//	GetLocalTime(&t);
//	char strtime[64] = { 0 };
//
//	sprintf_s(strtime, sizeof(strtime), "%04d-%02d-%02d %02d:%02d:%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
//
//	char buffer[2048];
//	int count = 0;
//	int endpos = 0;
//	response_pkt_p pkt;
//
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$log,%s,%s,{\"taskid\":\"%s\",\"executor\":\"%s\",\"net\":\"%s\",\"data\":{\"time\":\"%s\",\"type\":\"%s\"},\"loc\":{\"lon\":\"%s\",\"lat\":\"%s\",\"hgt\":\"%s\"}}", "frontsvr", "monitor", taskid, executor, net, strtime, "taskcontinue", lon, lat, hgt);
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
//	endpos = count;
//	buffer[endpos] = '\0';
//
//	p2p = (p2p_t)destsess;
//	pkt = response_pkt_new(client()->pktpool, count);
//	memcpy(pkt->data, buffer, count);
//	pkt->len = count;
//	//pkt->data[pkt->len] = '\0';
//	p2p_stream_push(p2p, pkt);
//
//	log_write(p2p->sess.client->log, LOG_NOTICE, "send log[taskcontinue] to monitor:%s.", buffer);
//	cJSON_Delete(jsonroot);
//}
//
//void forward_taskdone_to_monitor(char *jstr)
//{
//	p2p_t p2p = NULL;
//	sess_t destsess;
//
//	char *monitor = "monitor";
//	destsess = getrefsvr(monitor);
//
//	if (destsess == NULL)
//	{
//		return;
//	}
//
//	// 先把taskdone请求直接转发给monitor
//	int cnt = 0;
//	char buffer[2048] = { 0 };
//	response_pkt_p pkt = NULL;
//	unsigned char ucheck;
//	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$taskdone,%s,%s,%s", "frontsvr", "monitor", jstr);
//	ucheck = checksum((unsigned char*)buffer, cnt);
//	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
//	buffer[cnt] = '\0';
//	p2p = (p2p_t)destsess;
//	pkt = response_pkt_new(client()->pktpool, cnt);
//	memcpy(pkt->data, buffer, cnt);
//	pkt->len = cnt;
//	//pkt->data[pkt->len] = '\0';
//	p2p_stream_push(p2p, pkt);
//
//	log_write(p2p->sess.client->log, LOG_NOTICE, "send taskdone to monitor:%s.", buffer);
//}
//
//void forward_log_taskdone_to_monitor(char *jstr)
//{
//	p2p_t p2p = NULL;
//	sess_t destsess;
//
//	char *monitor = "monitor";
//	destsess = getrefsvr(monitor);
//
//	if (destsess == NULL)
//	{
//		return;
//	}
//
//
//	cJSON *jsonroot, *json;
//	jsonroot = cJSON_Parse(jstr);
//	if (jsonroot == NULL)
//		return;
//	char *taskid, *executor, *net;
//
//	json = cJSON_GetObjectItem(jsonroot, "taskid");
//	if (json == NULL || json->valuestring == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	taskid = json->valuestring;
//
//	json = cJSON_GetObjectItem(jsonroot, "executor");
//	if (json == NULL || json->valuestring == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	executor = json->valuestring;
//
//	json = cJSON_GetObjectItem(jsonroot, "net");
//	if (json == NULL || json->valuestring == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	net = json->valuestring;
//
//	json = cJSON_GetObjectItem(jsonroot, "loc");
//	if (json == NULL || json->valuestring == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//
//	cJSON* item = cJSON_GetObjectItem(json, "lon");
//	if (item == NULL || item->valuestring == NULL)
//	{
//		cJSON_Delete(jsonroot);
//		return -1;
//	}
//	char *lon = item->valuestring;
//
//	item = cJSON_GetObjectItem(json, "lat");
//	if (item == NULL || item->valuestring == NULL)
//	{
//		cJSON_Delete(jsonroot);
//		return -1;
//	}
//	char *lat = item->valuestring;
//
//	item = cJSON_GetObjectItem(json, "hgt");
//	if (item == NULL || item->valuestring == NULL)
//	{
//		cJSON_Delete(jsonroot);
//		return -1;
//	}
//	char *hgt = item->valuestring;
//
//	SYSTEMTIME t;
//	GetLocalTime(&t);
//	char strtime[64] = { 0 };
//
//	sprintf_s(strtime, sizeof(strtime), "%04d-%02d-%02d %02d:%02d:%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
//
//	char buffer[2048];
//	int count = 0;
//	int endpos = 0;
//	response_pkt_p pkt;
//
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$log,%s,%s,{\"taskid\":\"%s\",\"executor\":\"%s\",\"net\":\"%s\",\"data\":{\"time\":\"%s\",\"type\":\"%s\"},\"loc\":{\"lon\":\"%s\",\"lat\":\"%s\",\"hgt\":\"%s\"}}", "frontsvr", "monitor", taskid, executor, net, strtime, "taskdone", lon, lat, hgt);
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
//	endpos = count;
//	buffer[endpos] = '\0';
//
//	p2p = (p2p_t)destsess;
//	pkt = response_pkt_new(client()->pktpool, count);
//	memcpy(pkt->data, buffer, count);
//	pkt->len = count;
//	//pkt->data[pkt->len] = '\0';
//	p2p_stream_push(p2p, pkt);
//
//	log_write(p2p->sess.client->log, LOG_NOTICE, "send log[taskdone] log to monitor:%s.", buffer);
//	cJSON_Delete(jsonroot);
//}
//
//void forward_taskpau_ack_to_app(char *jstr)
//{
//	sess_t destsess;
//	p2p_t p2p;
//
//	cJSON *jsonroot, *json;
//	jsonroot = cJSON_Parse(jstr);
//	if (jsonroot == NULL)
//		return;
//	char *net, *executor;
//
//	json = cJSON_GetObjectItem(jsonroot, "net");
//	if (json == NULL || json->valuestring == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	net = json->valuestring;
//
//	// 如果网络类型是北斗，则直接转发给bdsvr
//	if (strcmp("bd", net) == 0)
//	{
//		char *bdsvr = "bdsvr";
//		destsess = getrefsvr(bdsvr);
//	}
//	else
//	{
//		json = cJSON_GetObjectItem(jsonroot, "executor");
//		if (json == NULL || json->valuestring == NULL) {
//			cJSON_Delete(jsonroot);
//			return;
//		}
//		executor = json->valuestring;
//		destsess = getrefsvr(executor);		// 如果是互联网类型，则根据用户名查找对应连接
//	}
//
//	if (destsess == NULL)
//	{
//		cJSON_Delete(jsonroot);
//		return;
//	}
//
//	int cnt = 0;
//	char buffer[2048] = { 0 };
//	response_pkt_p pkt = NULL;
//	unsigned char ucheck;
//	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$taskpau,%s,%s,%s", "frontsvr", destsess->sname, jstr);
//	ucheck = checksum((unsigned char*)buffer, cnt);
//	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
//	p2p = (p2p_t)destsess;
//	pkt = response_pkt_new(client()->pktpool, cnt);
//	memcpy(pkt->data, buffer, cnt);
//	pkt->len = cnt;
//	pkt->data[pkt->len] = '\0';
//	p2p_stream_push(p2p, pkt);
//
//	log_write(client()->log, LOG_NOTICE, "send taskpau ack to %s:%s\n", destsess->sname, buffer);
//	cJSON_Delete(jsonroot);
//}
//
//void forward_taskdone_ack_to_warnsvr(char *jstr)
//{
//	char buffer[2048];
//	int count = 0;
//	int endpos = 0;
//	response_pkt_p pkt;
//
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$taskdone,%s,%s,%s", "frontsvr", "warnsvr", jstr);
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
//	endpos = count;
//	buffer[endpos + 1] = '\0';
//
//	if (client()->p2p_warnsvr != NULL)
//	{
//		pkt = response_pkt_new(((p2pclient_t)(client()->p2p_warnsvr))->pktpool, endpos + 1);
//		memcpy(pkt->data, buffer, endpos + 1);
//		pkt->len = endpos + 1;
//		p2pclient_write((p2pclient_t)(client()->p2p_warnsvr), pkt);
//
//		log_write(client()->log, LOG_NOTICE, "send taskdone ack to warnsvr:%s.", buffer);
//	}
//}
//
//void forward_taskdone_ack_to_app(char *jstr)
//{
//	sess_t destsess;
//	p2p_t p2p;
//
//	cJSON *jsonroot, *json;
//	jsonroot = cJSON_Parse(jstr);
//	if (jsonroot == NULL)
//		return;
//	char *net, *executor;
//
//	json = cJSON_GetObjectItem(jsonroot, "net");
//	if (json == NULL || json->valuestring == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	net = json->valuestring;
//
//	// 如果网络类型是北斗，则直接转发给bdsvr
//	if (strcmp("bd", net) == 0)
//	{
//		char *bdsvr = "bdsvr";
//		destsess = getrefsvr(bdsvr);
//	}
//	else
//	{
//		json = cJSON_GetObjectItem(jsonroot, "executor");
//		if (json == NULL || json->valuestring == NULL) {
//			cJSON_Delete(jsonroot);
//			return;
//		}
//		executor = json->valuestring;
//		destsess = getrefsvr(executor);		// 如果是互联网类型，则根据用户名查找对应连接
//	}
//
//	if (destsess == NULL)
//	{
//		cJSON_Delete(jsonroot);
//		return;
//	}
//
//	int cnt = 0;
//	char buffer[2048] = { 0 };
//	response_pkt_p pkt = NULL;
//	unsigned char ucheck;
//	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$taskdone,%s,%s,%s", "frontsvr", destsess->sname, jstr);
//	ucheck = checksum((unsigned char*)buffer, cnt);
//	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
//	p2p = (p2p_t)destsess;
//	pkt = response_pkt_new(client()->pktpool, cnt);
//	memcpy(pkt->data, buffer, cnt);
//	pkt->len = cnt;
//	pkt->data[pkt->len] = '\0';
//	p2p_stream_push(p2p, pkt);
//
//	log_write(client()->log, LOG_NOTICE, "send taskdone ack to %s:%s\n", destsess->sname, buffer);
//	cJSON_Delete(jsonroot);
//}
//
//void forward_status_to_monitor(char *jstr)
//{
//	p2p_t p2p = NULL;
//	sess_t destsess;
//	char *monitor = "monitor";
//	destsess = getrefsvr(monitor);
//
//	if (destsess == NULL)
//	{
//		return;
//	}
//
//	int cnt = 0;
//	char buffer[2048] = { 0 };
//	response_pkt_p pkt = NULL;
//	unsigned char ucheck;
//	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$sta,%s,%s,%s", "frontsvr", "monitor", jstr);
//	ucheck = checksum((unsigned char*)buffer, cnt);
//	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
//	buffer[cnt] = '\0';
//	p2p = (p2p_t)destsess;
//	pkt = response_pkt_new(client()->pktpool, cnt);
//	memcpy(pkt->data, buffer, cnt);
//	pkt->len = cnt;
//	//pkt->data[pkt->len] = '\0';
//	p2p_stream_push(p2p, pkt);
//
//	log_write(p2p->sess.client->log, LOG_NOTICE, "send status to monitor:%s.", buffer);
//}
//
//void forward_log_to_monitor(char *jstr)
//{
//	p2p_t p2p = NULL;
//	sess_t destsess;
//	char *monitor = "monitor";
//	destsess = getrefsvr(monitor);
//
//	if (destsess == NULL)
//	{
//		return;
//	}
//
//	int cnt = 0;
//	char buffer[2048] = { 0 };
//	response_pkt_p pkt = NULL;
//	unsigned char ucheck;
//	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$log,%s,%s,%s", "frontsvr", "monitor", jstr);
//	ucheck = checksum((unsigned char*)buffer, cnt);
//	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
//	buffer[cnt] = '\0';
//	p2p = (p2p_t)destsess;
//	pkt = response_pkt_new(client()->pktpool, cnt);
//	memcpy(pkt->data, buffer, cnt);
//	pkt->len = cnt;
//	//pkt->data[pkt->len] = '\0';
//	p2p_stream_push(p2p, pkt);
//
//	log_write(p2p->sess.client->log, LOG_NOTICE, "send orig log to monitor:%s.", buffer);
//}
//
//void forward_pos_to_warnsvr(char *jstr)
//{
//	char buffer[2048];
//	int count = 0;
//	int endpos = 0;
//	response_pkt_p pkt;
//
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$pos,%s,%s,%s", "frontsvr", "warnsvr", jstr);
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
//	endpos = count;
//	buffer[endpos + 1] = '\0';
//
//	p2pclient_t conn = client()->p2p_warnsvr;
//	if (conn != NULL && conn->lost == FALSE)
//	{
//		pkt = response_pkt_new(conn->pktpool, endpos + 1);
//		memcpy(pkt->data, buffer, endpos + 1);
//		pkt->len = endpos + 1;
//
//		int rt = p2pclient_write(conn, pkt);
//
//		if (rt == -1)
//			response_pkt_free(pkt);
//		else
//			log_write(client()->log, LOG_NOTICE, "send pos to warnsvr:%s.", buffer);
//	}
//}
//

//
//void forward_pos_to_monitor(char *jstr)
//{
//	p2p_t p2p = NULL;
//	sess_t destsess;
//
//	char *monitor = "monitor";
//	destsess = getrefsvr(monitor);
//	if (destsess == NULL)
//	{
//		return;
//	}
//
//	int cnt = 0;
//	char buffer[2048] = { 0 };
//	response_pkt_p pkt = NULL;
//	unsigned char ucheck;
//	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$pos,%s,%s,%s", "frontsvr", "monitor", jstr);
//	ucheck = checksum((unsigned char*)buffer, cnt);
//	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
//	buffer[cnt] = '\0';
//	p2p = (p2p_t)destsess;
//	pkt = response_pkt_new(client()->pktpool, cnt);
//	memcpy(pkt->data, buffer, cnt);
//	pkt->len = cnt;
//	//pkt->data[pkt->len] = '\0';
//	p2p_stream_push(p2p, pkt);
//
//	log_write(p2p->sess.client->log, LOG_NOTICE, "send pos to monitor:%s.", buffer);
//}
//
//void forward_taskpau_ack_to_warnsvr(char *jstr)
//{
//	char buffer[2048];
//	int count = 0;
//	int endpos = 0;
//	response_pkt_p pkt;
//
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$taskpau,%s,%s,%s", "frontsvr", "warnsvr", jstr);
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
//	endpos = count;
//	buffer[endpos + 1] = '\0';
//
//	if (client()->p2p_warnsvr != NULL)
//	{
//		pkt = response_pkt_new(((p2pclient_t)(client()->p2p_warnsvr))->pktpool, endpos + 1);
//		memcpy(pkt->data, buffer, endpos + 1);
//		pkt->len = endpos + 1;
//		p2pclient_write((p2pclient_t)(client()->p2p_warnsvr), pkt);
//
//		log_write(client()->log, LOG_NOTICE, "send taskpau ack to warnsvr:%s.", buffer);
//	}
//}
//
//void forward_taskcon_ack_to_warnsvr(char *jstr)
//{
//	char buffer[2048];
//	int count = 0;
//	int endpos = 0;
//	response_pkt_p pkt;
//
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$taskcon,%s,%s,%s", "frontsvr", "warnsvr", jstr);
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
//	endpos = count;
//	buffer[endpos + 1] = '\0';
//
//	if (client()->p2p_warnsvr != NULL)
//	{
//		pkt = response_pkt_new(((p2pclient_t)(client()->p2p_warnsvr))->pktpool, endpos + 1);
//		memcpy(pkt->data, buffer, endpos + 1);
//		pkt->len = endpos + 1;
//		p2pclient_write((p2pclient_t)(client()->p2p_warnsvr), pkt);
//
//		log_write(client()->log, LOG_NOTICE, "send taskcon ack to warnsvr:%s.", buffer);
//	}
//}
//
//void forward_taskcon_ack_to_app(char *jstr)
//{
//	sess_t destsess;
//	p2p_t p2p; 
//
//	cJSON *jsonroot, *json;
//	jsonroot = cJSON_Parse(jstr);
//	if (jsonroot == NULL)
//		return;
//	char *net, *executor;
//
//	json = cJSON_GetObjectItem(jsonroot, "net");
//	if (json == NULL || json->valuestring == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	net = json->valuestring;
//
//	// 如果网络类型是北斗，则直接转发给bdsvr
//	if (strcmp("bd", net) == 0)
//	{
//		char *bdsvr = "bdsvr";
//		destsess = getrefsvr(bdsvr);
//	}
//	else
//	{
//		json = cJSON_GetObjectItem(jsonroot, "executor");
//		if (json == NULL || json->valuestring == NULL) {
//			cJSON_Delete(jsonroot);
//			return;
//		}
//		executor = json->valuestring;
//		destsess = getrefsvr(executor);		// 如果是互联网类型，则根据用户名查找对应连接
//	}
//
//	if (destsess == NULL)
//	{
//		cJSON_Delete(jsonroot);
//		return;
//	}
//
//	int cnt = 0;
//	char buffer[2048] = { 0 };
//	response_pkt_p pkt = NULL;
//	unsigned char ucheck;
//	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$taskcon,%s,%s,%s", "frontsvr", destsess->sname, jstr);
//	ucheck = checksum((unsigned char*)buffer, cnt);
//	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
//	p2p = (p2p_t)destsess;
//	pkt = response_pkt_new(client()->pktpool, cnt);
//	memcpy(pkt->data, buffer, cnt);
//	pkt->len = cnt;
//	pkt->data[pkt->len] = '\0';
//	p2p_stream_push(p2p, pkt);
//
//	log_write(client()->log, LOG_NOTICE, "send taskcon ack to %s:%s\n", destsess->sname, buffer);
//	cJSON_Delete(jsonroot);
//}
