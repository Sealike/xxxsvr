#include "warnsvr_callback.h"
#include "p2p.h"
#include "protobuffer.h"
#include "warnsvr_expat.h"
#include "client.h"
#include "tool.h"
#include "myquery/mysqldb.h"
#include "json/cJSON.h"
#include "dbaccess.h"
#include "p2pclient.h"
#include "forward.h"
#include "getrefsvr.h"


//static sess_t getrefsvr(const char* cname)
//{
//	if (client()->sessions == NULL) return NULL;
//	sess_t rtn = (sess_t)xhash_get(client()->sessions, cname);
//
//	return rtn;
//}


static unsigned char checksum(unsigned char buffer[], int len)
{
	unsigned char data = 0;
	int i = 0;
	for (; i < len; i++)
		data ^= buffer[i];

	return data;
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

	log_write(client()->log, LOG_NOTICE, "save_x_log2.");
	json = cJSON_GetObjectItem(jsonroot, "executor");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	executor = json->valuestring;

	log_write(client()->log, LOG_NOTICE, "save_x_log3.");
	json = cJSON_GetObjectItem(jsonroot, "net");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	net = json->valuestring;

	log_write(client()->log, LOG_NOTICE, "save_x_log4.");

	//////////////////////////////////////////////////////////////////////////
	// 解析data
	json = cJSON_GetObjectItem(jsonroot, "data");
	if (json == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}

	log_write(client()->log, LOG_NOTICE, "save_x_log5.");

	cJSON* item = cJSON_GetObjectItem(json, "desc");
	/*if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}
	char *desc = item->valuestring;
*/
	char *desc = "";
	if (item != NULL && item->valuestring != NULL)
	{
		char *desc = item->valuestring;
	}


	log_write(client()->log, LOG_NOTICE, "save_x_log6.");
	//////////////////////////////////////////////////////////////////////////
	//解析loc
	json = cJSON_GetObjectItem(jsonroot, "loc");
	if (json == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}


	log_write(client()->log, LOG_NOTICE, "save_x_log6.");

	item = cJSON_GetObjectItem(json, "lon");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}
	char *lon = item->valuestring;

	log_write(client()->log, LOG_NOTICE, "save_x_log7.");

	item = cJSON_GetObjectItem(json, "lat");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}
	char *lat = item->valuestring;

	log_write(client()->log, LOG_NOTICE, "save_x_log8.");

	item = cJSON_GetObjectItem(json, "hgt");
	if (item == NULL || item->valuestring == NULL)
	{
		cJSON_Delete(jsonroot);
		return;
	}
	char *hgt = item->valuestring;

	log_write(client()->log, LOG_NOTICE, "save_x_log9.");
	SYSTEMTIME t;
	GetLocalTime(&t);
	char strtime[64] = { 0 };

	sprintf_s(strtime, sizeof(strtime), "%04d-%02d-%02d %02d:%02d:%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

	log_write(client()->log, LOG_NOTICE, "save_x_log10.");
	log_write(client()->log, LOG_NOTICE, "save_x_log: save [%s] log:%s\n", type, jstr);


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

	cJSON_Delete(jsonroot);
}

static auth_hook(p2pclient_t p2pclient, char *jstr)
{
	cJSON *jsonroot, *json;
	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL)
		return;
	char *rslt, *error;

	json = cJSON_GetObjectItem(jsonroot, "rslt");
	if (json == NULL || json->valuestring == NULL) {
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

	log_write(client()->log, LOG_NOTICE, "recv auth ok from warnsvr.");
}

static void initack_hook(sess_t sess,char *jstr)
{
	cJSON *jsonroot, *json;

	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL) return;

	strncpy_s(sess->sname, sizeof(sess->sname), "warnsvr", strlen("warnsvr"));

	json = cJSON_GetObjectItem(jsonroot, "role");
	if (json == NULL || json->valuestring == NULL)
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

	init_sess_st authpara;
	strcpy_s(authpara.from, sizeof(authpara.from) - 1, "frontsvr");
	strcpy_s(authpara.to, sizeof(authpara.to) - 1, "warnsvr");
	protobuffer_send(client()->p2p_warnsvr, eProto_auth, &authpara);
}

// 将来自warnsvr的告警信息（电子围栏、任务超期、消失等）解析分段并入库
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

	cJSON_Delete(jsonroot);
}


void warnsvr_exp_cb(int msgid, void* msg, int len, void* param)
{
	sess_t sess,destsess;
	unsigned char* frame = (unsigned char*)msg;

	warnsvr_exp_t exp = (warnsvr_exp_t)param;
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
		// 收到来自warnsvr的initack消息，如果内容为ok，则发送auth消息
		log_write(client()->log, LOG_NOTICE, "recv initack from %s .\n", sess->sname);	 //提示收到Initsession回复
		initack_hook(sess, jstr);
	}

	case cunpack_pong:
	{
		log_write(client()->log, LOG_NOTICE, "==== pong from warnsvr. ====");
	}
	break;

	case cunpack_auth:
	{
		// 收到auth回复，如果ok则继续，否则主动断开连接
		log_write(client()->log, LOG_NOTICE, "recv auth ack from %s .\n", sess->sname);	 //提示收到Initsession回复
		auth_hook(p2pclient, jstr);
	}
	break;

	case cunpack_warning:
	{
		log_write(client()->log, LOG_NOTICE, "recv warn from warnsvr:%s.", (char*)frame);	
		// forward_warn_to_monitor(jstr);
		// forward_warn_to_app(jstr);	

		forward_to_monitor("warn", jstr);
		forward_to_app("warn", jstr);		// 由于北斗频度受限，只通过4G发送告警信息
		save_warning(jstr);
	}
	break;

	case cunpack_dyncode:
	{
		log_write(client()->log, LOG_NOTICE, "recv dyncode from warnsvr:%s.", (char*)frame);
		//forward_dyncode_to_app(jstr);
		//forward_log_dyncode_to_monitor(jstr);

		forward_to_app("dyncode", jstr);
		//save_x_log("dyncode",jstr);
		forward_log_x_to_monitor("dyncode", jstr);
	}
	break;

	case cunpack_verify:
	{
		log_write(client()->log, LOG_NOTICE, "recv verify from warnsvr:%s.", (char*)frame);
		
		forward_to_monitor("verify", jstr);
		//save_x_log("verify",jstr);
		forward_log_x_to_monitor("verify", jstr);
	}
	break;

	default:
		break;
	}
}
