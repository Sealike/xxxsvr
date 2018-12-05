#include "frontsvr_callback.h"
#include "p2p.h"
#include "protobuffer.h"
#include "frontsvr_expat.h"
#include "client.h"
#include "tool.h"
#include "myquery/mysqldb.h"
#include "json/cJSON.h"
#include "dbaccess.h"
#include "p2pclient.h"

extern xht g_xsubbytaskid;
extern xht g_xtaskidexecutor;

static sess_t getrefsvr(const char* cname)
{
	if (client()->sessions == NULL) return NULL;
	sess_t rtn = (sess_t)xhash_get(client()->sessions, cname);

	return rtn;
}

static void xhash_walk_throgh_2(xht h, const char *key, void *val, void *arg)
{
	if (val != NULL)
	{
		int rtn = 0;
		printf("%s-%p\n", key, val);
	}
}

static xhash_walker xhash_walk1(xht h, const char *key, void *val, void *arg)
{
	printf("key:%s val:%p\n", key, val);
}



static unsigned char checksum(unsigned char buffer[], int len)
{
	unsigned char data = 0;
	int i = 0;
	for (; i < len; i++)
		data ^= buffer[i];

	return data;
}

static void initack_hook(sess_t sess, char *jstr)
{
	cJSON *jsonroot, *json;

	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL) return;

	json = cJSON_GetObjectItem(jsonroot, "role");
	if (json == NULL || json->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}
	//strncpy_s(sess->sname, sizeof(sess->sname), json->valuestring, sizeof(sess->sname));
	strncpy_s(sess->sname, sizeof(sess->sname), "frontsvr", strlen("frontsvr"));

	if (strcmp(json->valuestring, "monitor") != 0)			//不是发给自身的Init信息
	{
		cJSON_Delete(jsonroot);
		return;
	}

	json = cJSON_GetObjectItem(jsonroot, "rslt");
	if (json == NULL || json->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}

	if (strcmp(json->valuestring, "ok") != 0)				// 发给自己，但是init失败 
	{
		cJSON_Delete(jsonroot);
		p2pclient_close((p2pclient_t)sess);				// 关闭连接
		return;
	}

	cJSON_Delete(jsonroot);

	sess->binitsess = TRUE;
	log_write(client()->log, LOG_NOTICE, "recv initack ok from %s.", sess->sname);	 //提示收到Initsession回复

	init_sess_st authpara;
	strcpy_s(authpara.from, sizeof(authpara.from) - 1, "monitor");
	strcpy_s(authpara.to, sizeof(authpara.to) - 1, "frontsvr");
	protobuffer_send(client()->p2p_frontsvr, eProto_auth, &authpara);
}

static void authack_hook(p2pclient_t p2pclient, char *jstr)
{
	cJSON *jsonroot, *json;
	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL) return;
	char *rslt;

	json = cJSON_GetObjectItem(jsonroot, "rslt");
	if (json == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	rslt = json->valuestring;

	if (strcmp(rslt, "fail") == 0)
	{
		p2pclient_close(p2pclient);
		cJSON_Delete(jsonroot);
		return;
	}

	log_write(client()->log, LOG_NOTICE, "recv auth ok from frontsvr.");
	cJSON_Delete(jsonroot);
}


static void forward_to_monitclient(char *head, char *jstr)
{
	cJSON *jsonroot, *json;
	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL) return;
	char *taskid, *executor;

	json = cJSON_GetObjectItem(jsonroot, "taskid");
	if (json == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	taskid = json->valuestring;

	/*json = cJSON_GetObjectItem(jsonroot, "executor");
	if (json == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	executor = json->valuestring;
	char * key_taskid = _strdup(taskid);
	xhash_zap(g_xtaskidexecutor, key_taskid);
	xhash_put(g_xtaskidexecutor, key_taskid, executor);*/

	int cnt = 0;
	char buffer[2048];
	response_pkt_p pkt = NULL;
	unsigned char ucheck;

	jqueue_t q = (jqueue_t)xhash_get(g_xsubbytaskid, taskid);
	if (q == NULL)
	{
		log_write(client()->log, LOG_NOTICE, "Nobody has subscribed %s of [taskid=%s]", head, taskid);
		cJSON_Delete(jsonroot);
		return;
	}
	int q_cnt = jqueue_size(q);
	int i = 0;
	sess_t destsess;
	for (i = 0; i < q_cnt; i++)
	{
		destsess = (sess_t)jqueue_pull(q);
		jqueue_push(q, destsess, 0);

		cnt = 0;
		memset(buffer, 0, sizeof(buffer));
		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$%s,%s,%s,%s", head, "monitor", destsess->sname, jstr);
		ucheck = checksum((unsigned char*)buffer, cnt);
		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
		buffer[cnt] = '\0';
		p2p_t p2p = (p2p_t)destsess;
		pkt = response_pkt_new(client()->pktpool, cnt);
		memcpy(pkt->data, buffer, cnt);
		pkt->len = cnt;
		//pkt->data[pkt->len] = '\0';
		p2p_stream_push(p2p, pkt);

		log_write(client()->log, LOG_NOTICE, "send %s to %s:%s.", head,destsess->sname, buffer);
	}
	cJSON_Delete(jsonroot);
}

//
//static void forward_pos_to_monitor(char *jstr)
//{
//	cJSON *jsonroot, *json;
//	jsonroot = cJSON_Parse(jstr);
//	if (jsonroot == NULL) return;
//	char *taskid;
//
//	json = cJSON_GetObjectItem(jsonroot, "taskid");
//	if (json == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	taskid = json->valuestring;
//
//	int cnt = 0;
//	char buffer[2048];
//	response_pkt_p pkt = NULL;
//	unsigned char ucheck;
//
//	jqueue_t q = (jqueue_t)xhash_get(g_xsubbytaskid, taskid);
//	if (q == NULL)
//	{
//		log_write(client()->log, LOG_NOTICE, "Nobody has subscribed positions of [taskid=%s]", taskid);
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	int q_cnt = jqueue_size(q);
//	int i = 0;
//	sess_t destsess;
//	for (i = 0; i < q_cnt; i++)
//	{
//		destsess = (sess_t)jqueue_pull(q);
//		jqueue_push(q, destsess, 0);
//
//		cnt = 0;
//		memset(buffer, 0, sizeof(buffer));
//		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$pos,%s,%s,%s", "monitor", destsess->sname, jstr);
//		ucheck = checksum((unsigned char*)buffer, cnt);
//		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
//		buffer[cnt] = '\0';
//		p2p_t p2p = (p2p_t)destsess;
//		pkt = response_pkt_new(client()->pktpool, cnt);
//		memcpy(pkt->data, buffer, cnt);
//		pkt->len = cnt;
//		//pkt->data[pkt->len] = '\0';
//		p2p_stream_push(p2p, pkt);
//
//		log_write(client()->log, LOG_NOTICE, "send pos to %s:%s.", destsess->sname, buffer);
//	}
//	cJSON_Delete(jsonroot);
//}
//
//static void forward_warn_to_monitor(char * jstr)
//{
//	cJSON *jsonroot, *json;
//	jsonroot = cJSON_Parse(jstr);
//	if (jsonroot == NULL) return;
//	char *taskid;
//
//	json = cJSON_GetObjectItem(jsonroot, "taskid");
//	if (json == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	taskid = json->valuestring;
//
//	int cnt = 0;
//	char buffer[2048];
//	response_pkt_p pkt = NULL;
//	unsigned char ucheck;
//
//	jqueue_t q = (jqueue_t)xhash_get(g_xsubbytaskid, taskid);
//	//xhash_walk(g_xsubbytaskid, xhash_walk1, NULL);
//	if (q == NULL)
//	{
//		log_write(client()->log, LOG_NOTICE, "Nobody has subscribed warnings of [taskid=%s]", taskid);
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	int q_cnt = jqueue_size(q);
//	int i = 0;
//	sess_t destsess;
//	for (i = 0; i < q_cnt; i++)
//	{
//		destsess = (sess_t)jqueue_pull(q);
//		jqueue_push(q, destsess, 0);
//
//		cnt = 0;
//		memset(buffer, 0, sizeof(buffer));
//		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$warn,%s,%s,%s", "monitor", destsess->sname, jstr);
//		ucheck = checksum((unsigned char*)buffer, cnt);
//		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
//		buffer[cnt] = '\0';
//		p2p_t p2p = (p2p_t)destsess;
//		pkt = response_pkt_new(client()->pktpool, cnt);
//		memcpy(pkt->data, buffer, cnt);
//		pkt->len = cnt;
//		//pkt->data[pkt->len] = '\0';
//		p2p_stream_push(p2p, pkt);
//
//		log_write(client()->log, LOG_NOTICE, "send warn to %s:%s.", destsess->sname, buffer);
//	}
//	cJSON_Delete(jsonroot);
//}
//
//static void forward_log_to_monitor(char *jstr)
//{
//	cJSON *jsonroot, *json;
//	jsonroot = cJSON_Parse(jstr);
//	if (jsonroot == NULL) return;
//	char *taskid;
//
//	json = cJSON_GetObjectItem(jsonroot, "taskid");
//	if (json == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	taskid = json->valuestring;
//
//	int cnt = 0;
//	char buffer[2048];
//	response_pkt_p pkt = NULL;
//	unsigned char ucheck;
//
//	jqueue_t q = (jqueue_t)xhash_get(g_xsubbytaskid, taskid);
//	if (q == NULL)
//	{
//		log_write(client()->log, LOG_NOTICE, "Nobody has subscribed logs of [taskid=%s]", taskid);
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	int q_cnt = jqueue_size(q);
//	int i = 0;
//	sess_t destsess;
//	for (i = 0; i < q_cnt; i++)
//	{
//		destsess = (sess_t)jqueue_pull(q);
//		jqueue_push(q, destsess, 0);
//
//		cnt = 0;
//		memset(buffer, 0, sizeof(buffer));
//		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$log,%s,%s,%s", "monitor", destsess->sname, jstr);
//		ucheck = checksum((unsigned char*)buffer, cnt);
//		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
//		buffer[cnt] = '\0';
//		p2p_t p2p = (p2p_t)destsess;
//		pkt = response_pkt_new(client()->pktpool, cnt);
//		memcpy(pkt->data, buffer, cnt);
//		pkt->len = cnt;
//		//pkt->data[pkt->len] = '\0';
//		p2p_stream_push(p2p, pkt);
//
//		log_write(client()->log, LOG_NOTICE, "send log to %s:%s\n", destsess->sname, buffer);
//	}
//	cJSON_Delete(jsonroot);
//}
//
//static void forward_taskpau_to_monitor(char *jstr)
//{
//	cJSON *jsonroot, *json;
//	jsonroot = cJSON_Parse(jstr);
//	if (jsonroot == NULL) return;
//	char *taskid;
//
//	json = cJSON_GetObjectItem(jsonroot, "taskid");
//	if (json == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	taskid = json->valuestring;
//
//	int cnt = 0;
//	char buffer[2048];
//	response_pkt_p pkt = NULL;
//	unsigned char ucheck;
//
//	jqueue_t q = (jqueue_t)xhash_get(g_xsubbytaskid, taskid);
//	if (q == NULL)
//	{
//		log_write(client()->log, LOG_NOTICE, "Nobody has subscribed taskpaus of [taskid=%s]", taskid);
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	int q_cnt = jqueue_size(q);
//	int i = 0;
//	sess_t destsess;
//	for (i = 0; i < q_cnt; i++)
//	{
//		destsess = (sess_t)jqueue_pull(q);
//		jqueue_push(q, destsess, 0);
//
//		cnt = 0;
//		memset(buffer, 0, sizeof(buffer));
//		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$taskpau,%s,%s,%s", "monitor", destsess->sname, jstr);
//		ucheck = checksum((unsigned char*)buffer, cnt);
//		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
//		buffer[cnt] = '\0';
//		p2p_t p2p = (p2p_t)destsess;
//		pkt = response_pkt_new(client()->pktpool, cnt);
//		memcpy(pkt->data, buffer, cnt);
//		pkt->len = cnt;
//		//pkt->data[pkt->len] = '\0';
//		p2p_stream_push(p2p, pkt);
//
//		log_write(client()->log, LOG_NOTICE, "send taskpau req to %s:%s.", destsess->sname, buffer);
//	}
//	cJSON_Delete(jsonroot);
//}
//
//static void forward_taskcon_to_monitor(char *jstr)
//{
//
//	cJSON *jsonroot, *json;
//	jsonroot = cJSON_Parse(jstr);
//	if (jsonroot == NULL) return;
//	char *taskid;
//
//	json = cJSON_GetObjectItem(jsonroot, "taskid");
//	if (json == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	taskid = json->valuestring;
//
//	int cnt = 0;
//	char buffer[2048];
//	response_pkt_p pkt = NULL;
//	unsigned char ucheck;
//
//	jqueue_t q = (jqueue_t)xhash_get(g_xsubbytaskid, taskid);
//	if (q == NULL)
//	{
//		log_write(client()->log, LOG_NOTICE, "Nobody has subscribed taskcons of [taskid=%s]", taskid);
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	int q_cnt = jqueue_size(q);
//	int i = 0;
//	sess_t destsess;
//	for (i = 0; i < q_cnt; i++)
//	{
//		destsess = (sess_t)jqueue_pull(q);
//		jqueue_push(q, destsess, 0);
//
//		cnt = 0;
//		memset(buffer, 0, sizeof(buffer));
//		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$taskcon,%s,%s,%s", "monitor", destsess->sname, jstr);
//		ucheck = checksum((unsigned char*)buffer, cnt);
//		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
//		buffer[cnt] = '\0';
//		p2p_t p2p = (p2p_t)destsess;
//		pkt = response_pkt_new(client()->pktpool, cnt);
//		memcpy(pkt->data, buffer, cnt);
//		pkt->len = cnt;
//		//pkt->data[pkt->len] = '\0';
//		p2p_stream_push(p2p, pkt);
//
//		log_write(client()->log, LOG_NOTICE, "send taskcon to %s:%s.", destsess->sname, buffer);
//	}
//	cJSON_Delete(jsonroot);
//}
//
//static void forward_status_to_monitor(char* jstr)
//{
//	cJSON *jsonroot, *json;
//	jsonroot = cJSON_Parse(jstr);
//	if (jsonroot == NULL) return;
//	char *taskid;
//
//	json = cJSON_GetObjectItem(jsonroot, "taskid");
//	if (json == NULL) {
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	taskid = json->valuestring;
//
//	int cnt = 0;
//	char buffer[2048];
//	response_pkt_p pkt = NULL;
//	unsigned char ucheck;
//
//	jqueue_t q = (jqueue_t)xhash_get(g_xsubbytaskid, taskid);
//	if (q == NULL)
//	{
//		log_write(client()->log, LOG_NOTICE, "Nobody has subscribed status of [taskid=%s]", taskid);
//		cJSON_Delete(jsonroot);
//		return;
//	}
//	int q_cnt = jqueue_size(q);
//	int i = 0;
//	sess_t destsess;
//	for (i = 0; i < q_cnt; i++)
//	{
//		destsess = (sess_t)jqueue_pull(q);
//		jqueue_push(q, destsess, 0);
//
//		cnt = 0;
//		memset(buffer, 0, sizeof(buffer));
//		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$sta,%s,%s,%s", "monitor", destsess->sname, jstr);
//		ucheck = checksum((unsigned char*)buffer, cnt);
//		cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
//		buffer[cnt] = '\0';
//		p2p_t p2p = (p2p_t)destsess;
//		pkt = response_pkt_new(client()->pktpool, cnt);
//		memcpy(pkt->data, buffer, cnt);
//		pkt->len = cnt;
//		//pkt->data[pkt->len] = '\0';
//		p2p_stream_push(p2p, pkt);
//
//		log_write(client()->log, LOG_NOTICE, "send status to %s:%s.", destsess->sname, buffer);
//	}
//	cJSON_Delete(jsonroot);
//}


static void configack_hook(char *jstr)
{
	cJSON *jsonroot, *json;
	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL)
		return;

	char *moniclient = NULL;

	json = cJSON_GetObjectItem(jsonroot, "account");
	if (json == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	moniclient = json->valuestring;

	if (moniclient == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}

	sess_t destsess;
	destsess = getrefsvr(moniclient);

	if (destsess == NULL)
	{
		return;
	}

	char buffer[1024];
	int cnt = 0;
	memset(buffer, 0, sizeof(buffer));

	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$%s,%s,%s,%s", "configack", "monitor", destsess->sname, jstr);
	int ucheck = checksum((unsigned char*)buffer, cnt);
	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
	buffer[cnt] = '\0';
	p2p_t p2p = (p2p_t)destsess;
	response_pkt_p pkt = response_pkt_new(client()->pktpool, cnt);
	memcpy(pkt->data, buffer, cnt);
	pkt->len = cnt;
	//pkt->data[pkt->len] = '\0';
	p2p_stream_push(p2p, pkt);

	log_write(client()->log, LOG_NOTICE, "send %s to %s:%s.", "configack", destsess->sname, buffer);

	cJSON_Delete(jsonroot);
}


void frontsvr_exp_cb(int msgid, void* msg, int len, void* param)
{
	sess_t sess;
	unsigned char* frame = (unsigned char*)msg;

	frontsvr_exp_t exp = (frontsvr_exp_t)param;
	sess = (sess_t)_BASE(exp).param;
	p2p_t p2p = (p2p_t)sess;
	p2pclient_t p2pclient = (p2pclient_t)sess;

	if (exp == NULL || sess == NULL) return;

	if (len - 5 < 0){
		p2pclient_close(p2pclient);
		return;
	}

	char *jstr = (char*)frame;
	jstr[len - 5] = '\0';
	while (*jstr != '{')jstr++;

	switch (msgid)
	{
	case cunpack_initack:
	{
		// 收到来自frontsvr的initack消息，如果结果是ok，则发送auth消息
		log_write(client()->log, LOG_NOTICE, "recv initack from %s.", sess->sname);
		initack_hook(sess,jstr);
	}

	case cunpack_pong:
	{		
		log_write(client()->log, LOG_NOTICE, "pong from frontsvr.");
	}
	break;

	case cunpack_auth:
	{
		// 收到auth回复，如果ok则继续，否则主动断开连接
		log_write(client()->log, LOG_NOTICE, "recv auth ack from %s.", sess->sname);
		authack_hook(p2pclient,jstr);
	}
	break;

	case cunpack_position:
	{
		log_write(client()->log, LOG_NOTICE, "recv position  from %s.", sess->sname);

		// 根据taskid查找订阅连接，依次发送
		//forward_pos_to_monitor(jstr);

		forward_to_monitclient("pos", jstr);
	}
	break;

	case cunpack_warning:
	{
		log_write(client()->log, LOG_NOTICE, "recv warning from %s.", sess->sname);
		
		// 根据taskid查找订阅连接，依次发送
		//forward_warn_to_monitor(jstr);
		forward_to_monitclient("warn", jstr);
	}
	break;

	case cunpack_log:
	{
		log_write(client()->log, LOG_NOTICE, "recv log from :%s.", sess->sname);
		
		// 根据taskid查找订阅连接，依次发送
		//forward_log_to_monitor(jstr);
		//db_save_log((mysqlquery_t)client()->sqlobj, jstr);
		forward_to_monitclient("log", jstr);
	}
	break;

	case cunpack_taskpau:
	{
		log_write(client()->log, LOG_NOTICE, "recv taskpau req from :%s.", sess->sname);
		
		// 根据taskid查找订阅连接，依次发送
		//forward_taskpau_to_monitor(jstr);
		forward_to_monitclient("taskpau", jstr);
	}
	break;

	case cunpack_taskcon:
	{
		log_write(client()->log, LOG_NOTICE, "recv taskcon req from :%s.", sess->sname);
		
		// 根据taskid查找订阅连接，依次发送
		//forward_taskcon_to_monitor(jstr);
		forward_to_monitclient("taskcon", jstr);
	}
	break;

	case cunpack_taskdone:
	{
		log_write(client()->log, LOG_NOTICE, "recv taskdone req from :%s.", sess->sname);

		// 根据taskid查找订阅连接，依次发送
		//forward_taskdone_to_monitor(jstr);
		forward_to_monitclient("taskdone", jstr);
	}
	break;

	case cunpack_status:
	{
		log_write(client()->log, LOG_NOTICE, "recv status from :%s.", sess->sname);
		
		// 根据taskid查找订阅连接，依次发送
		//forward_status_to_monitor(jstr);
		forward_to_monitclient("sta", jstr);
	}
	break;

	case cunpack_verify:
	{
		log_write(client()->log, LOG_NOTICE, "recv verify from :%s.", sess->sname);

		// 根据taskid查找订阅连接，依次发送
		forward_to_monitclient("verify", jstr);
	}
	break;

	case cunpack_configack:
	{
		log_write(client()->log, LOG_NOTICE, "recv configack from :%s.", sess->sname);

		configack_hook(jstr);
		
		


	}
	break;

	default:
		break;
	}
}
