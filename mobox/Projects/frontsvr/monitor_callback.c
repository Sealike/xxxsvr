#include "monitor_callback.h"
#include "p2p.h"
#include "protobuffer.h"
#include "monitor_expat.h"
#include "subst/subst.h"
#include "json/cJSON.h"
#include "client.h"
#include "forward.h"
#include "json_string.h"

extern int g_warntime;

static char* json_string(void* arg, int type)
{
	char  * rslt = NULL;

	cJSON * root = cJSON_CreateObject();

	switch (type)
	{
	case  DYNCODE:
	{
		dyncode_t dyncode = (dyncode_t)arg;

		cJSON_AddStringToObject(root, "taskid", dyncode->taskid);
		cJSON_AddStringToObject(root, "net", dyncode->net);
		cJSON_AddStringToObject(root, "executor", dyncode->executor);
		cJSON_AddStringToObject(root, "rslt", dyncode->rslt);

	}
	break;

	case CONFIGACK:
	{
		configack_t configack = (configack_t)arg;

		cJSON_AddStringToObject(root, "account", configack->account);
		cJSON_AddStringToObject(root, "rslt", configack->rslt);
		cJSON_AddStringToObject(root, "err", configack->err);
		cJSON_AddStringToObject(root, "seqno", configack->seqno);
	}
	break;

	default:
		break;
	}

	rslt = cJSON_PrintUnformatted(root);

	return rslt;
}

static sess_t getrefsvr(const char* cname)
{
	if (client()->sessions == NULL) return NULL;
	sess_t rtn = (sess_t)xhash_get(client()->sessions, cname);

	return rtn;
}

static unsigned char checksum(unsigned char buffer[], int len)
{
	unsigned char data = 0;
	int i = 0;
	for (; i < len; i++)
		data ^= buffer[i];

	return data;
}

static void authack_hook(sess_t sess, p2p_t p2p,char *jstr)
{
	cJSON *jsonroot, *json;
	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL)
		return;
	char *pwd, *name, *seqno, *net;

	json = cJSON_GetObjectItem(jsonroot, "name");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	name = json->valuestring;

	snprintf(sess->sname, sizeof(sess->sname), "%s", name);

	// 如果有旧连接，则将旧连接断开
	xht ht = client()->sessions;
	sess_t oldsess = NULL;
	oldsess = xhash_get(ht, sess->sname);
	if (oldsess != NULL)
	{
		printf("kill monitor oldsess %s\n", oldsess->sname);
		p2p_kill((p2p_t)oldsess);
	}

	json = cJSON_GetObjectItem(jsonroot, "seqno");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	seqno = json->valuestring;

	// 目前一律返回ok
	int cnt = 0;
	char buffer[2048] = { 0 };
	p2p = (p2p_t)sess;
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

	log_write(client()->log, LOG_NOTICE, "send auth ack to %s:%s\n", sess->sname, buffer);
}


// 从监控客户端返回的任务暂停请求应答，将其入库
static void save_taskpau_log(char *jstr)
{





}


static void verify_hook(char *jstr)
{
	cJSON *jsonroot, *json;
	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL)
		return;
	char *rslt;

	json = cJSON_GetObjectItem(jsonroot, "rslt");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	rslt = json->valuestring;

	if (strcmp(rslt, "deny") == 0)
	{
		// 授权申请被拒绝，则回复拒绝消息给app/bdsvr
		char *taskid, *net, *executor;

		dyncode_t dyncode = (dyncode_t)malloc(sizeof(dyncode_st));

		json = cJSON_GetObjectItem(jsonroot, "taskid");
		if (json == NULL || json->valuestring == NULL) {
			free(dyncode);
			cJSON_Delete(jsonroot);
			return;
		}
		taskid = json->valuestring;

		json = cJSON_GetObjectItem(jsonroot, "net");
		if (json == NULL || json->valuestring == NULL) {
			free(dyncode);
			cJSON_Delete(jsonroot);
			return;
		}
		net = json->valuestring;

		json = cJSON_GetObjectItem(jsonroot, "executor");
		if (json == NULL || json->valuestring == NULL) {
			free(dyncode);
			cJSON_Delete(jsonroot);
			return;
		}
		executor = json->valuestring;

		dyncode->taskid = taskid;
		dyncode->net = net;
		dyncode->executor = executor;
		dyncode->rslt = rslt;

		jstr = json_string(dyncode, DYNCODE);
		forward_to_app("dyncode", jstr);

		free(jstr);
		free(dyncode);
		
	}
	else
	{
		// 授权申请同意，则回复动态码给app/bdsvr
		forward_to_app("dyncode", jstr);
	}

	cJSON_Delete(jsonroot);

	return;
}


static void config_hook(char *jstr)
{
	cJSON *jsonroot, *json;
	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL)
		return;
	char *account = NULL;
	char *type = NULL;
	char *value = NULL;

	json = cJSON_GetObjectItem(jsonroot, "account");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	account = json->valuestring;
	if (NULL == account)
	{
		cJSON_Delete(jsonroot);
		return;
	}

	json = cJSON_GetObjectItem(jsonroot, "type");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	type = json->valuestring;
	if (NULL == type)
	{
		cJSON_Delete(jsonroot);
		return;
	}

	json = cJSON_GetObjectItem(jsonroot, "value");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	value = json->valuestring;
	if (NULL == value)
	{
		cJSON_Delete(jsonroot);
		return;
	}

	//strcpy_s(client()->plat_conf->warntime, strlen(value) + 1, value);
	//client()->plat_conf->warntime = atoi(value);

	// 存入t_config表，然后回复configack到monitor
	if (db_update_config(client()->sqlobj, atoi(value)) > 0)
	{
		g_warntime = atoi(value) * 60;
		configack_t configack = (configack_t)malloc(sizeof(configack_st));

		configack->account = account;
		configack->rslt = "ok";
		configack->err = "";
		configack->seqno = "";

		jstr = json_string(configack, CONFIGACK);
		forward_to_monitor("configack", jstr);
		free(configack);
		free(jstr);
	}

	cJSON_Delete(jsonroot);
	return;
}

//回复给前端：监控服务器
void  monitor_exp_cb(int msgid, void* msg, int len, void* param)
{
	sess_t sess;
	p2p_t  p2p;
	int  msgtype;
	int  msgcode = 0;
	unsigned char* frame = (unsigned char*)msg;
	monitor_exp_t exp = (monitor_exp_t)param;
	if (exp == NULL) return;
	sess = (sess_t)_BASE(exp).param;
	p2p = (p2p_t)sess;
	sess_t destsess;

	char * jstr = NULL;

	if (len - 5 < 0){
		p2p_kill(p2p);
		return;
	}

	jstr = (char*)frame;
	jstr[len - 5] = '\0';
	while (*jstr != '{')jstr++;

	switch (msgid)
	{
	case cunpack_ping:
	{
		log_write(client()->log, LOG_NOTICE, "recv ping from %s.",sess->sname);
		protobuffer_send_p2p(p2p, eProto_pong, sess->sname);
	}
	break;

	case cunpack_auth:
	{
		log_write(client()->log, LOG_NOTICE, "recv auth req from %s.", sess->sname);
		authack_hook(sess, p2p, jstr);
	}
	break;

	case cunpack_taskpau:
	{
		log_write(client()->log, LOG_NOTICE, "recv taskpau ack from %s.", sess->sname);
		// forward_taskpau_ack_to_warnsvr(jstr);
		// forward_taskpau_ack_to_app(jstr);

		// 来自管理客户端的请求回复，此时可以生成日志并入库
		forward_to_warnsvr("taskpau", jstr);
		forward_to_app("taskpau", jstr);
		//save_taskpau_log();
	}
	break;

	case cunpack_taskcon:
	{
		log_write(client()->log, LOG_NOTICE, "recv taskcon ack from %s.", sess->sname);
		// forward_taskcon_ack_to_warnsvr(jstr);
		// forward_taskcon_ack_to_app(jstr);	

		forward_to_warnsvr("taskcon", jstr);
		forward_to_app("taskcon", jstr);
	}
	break;

	case cunpack_taskdone:
	{
		log_write(client()->log, LOG_NOTICE, "recv taskcon ack from %s.", sess->sname);
		// forward_taskcon_ack_to_warnsvr(jstr);
		// forward_taskcon_ack_to_app(jstr);	

		forward_to_warnsvr("taskdone", jstr);
		forward_to_app("taskdone", jstr);
	}
	break;

	case cunpack_verify:
	{
		log_write(client()->log, LOG_NOTICE, "recv verify ack from %s.", sess->sname);
		
		// 解析判断，如果verify中rslt为ok，则拼成dyncode，发送给app/bdsvr
		verify_hook(jstr);
	}
	break;

	case cunpack_config:
	{
		log_write(client()->log, LOG_NOTICE, "recv config from %s.", sess->sname);

		config_hook(jstr);
	}
	break;

	case cunpack_remoteunlock:
	{
		log_write(client()->log, LOG_NOTICE, "recv config from %s.", sess->sname);


		forward_to_app("remoteunlock", jstr);	// 传给箱子后,箱子直接打开,然后生成remoteunlock类型的日志

		//  生成日志,存储,并回传给客户端

		forward_to_app("remoteunlock", jstr);				// 传给箱子后,箱子直接打开
		forward_log_x_to_monitor("remoteunlock", jstr);		// 生成相应日志,入库并发送回管理客户端

	}
	break;

	case cunpack_rptctrl:
	{
		log_write(client()->log, LOG_NOTICE, "recv config from %s.", sess->sname);


		forward_to_app("rptctrl", jstr);

		// 生成日志,存储,并回传给客户端

		forward_to_app("rptctrl", jstr);	
		forward_log_x_to_monitor("rptctrl", jstr);		// 生成相应日志,入库并发送回管理客户端

	}
	break;

	default:
		break;

	}
	
}

