#include "moniclient_callback.h"
#include "p2p.h"
#include "protobuffer.h"
#include "moniclient_expat.h"
#include "subst/subst.h"
#include "json/cJSON.h"
#include "client.h"
#include "subscribe.h"

extern xht g_xsubbytaskid;

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

static void auth_hook(sess_t sess, char *jstr)
{
	cJSON *jsonroot, *json;
	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL)
		return;
	char *pwd, *name, *seqno, *net;

	json = cJSON_GetObjectItem(jsonroot, "name");
	if (json == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	name = json->valuestring;

	snprintf(sess->sname, sizeof(sess->sname), "%s", name);

	json = cJSON_GetObjectItem(jsonroot, "seqno");
	if (json == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	seqno = json->valuestring;

	// 如果有旧连接，则将旧连接断开
	xht ht = client()->sessions;
	sess_t oldsess = NULL;
	oldsess = xhash_get(ht, sess->sname);
	/*if (oldsess != NULL)
	{
	printf("kill monitor oldsess %s\n", oldsess->sname);
	p2p_kill((p2p_t)oldsess);
	}*/

	if (oldsess != NULL)
	{
		xhash_zap(ht, sess->sname);
		auth_st auth;
		auth.ok = 1;
		strncpy(auth.peername, sess->sname, sizeof(auth.peername) - 1);
		strncpy(auth.err, "强制下线", sizeof(auth.err) - 1);
		auth.seqno = atoi(seqno);

		protobuffer_send_p2p((p2p_t)oldsess, eProto_offline, &auth);
		printf("kill client %s\n", oldsess->sname);
		p2p_kill((p2p_t)oldsess);
		//$offline, from, to, { "rslt":"ok", "error" : "被强制下线", "seqno" : "232" }*xx\r\n;
	}

	

	// 目前一律返回ok
	int cnt = 0;
	char buffer[2048] = { 0 };
	p2p_t p2p = (p2p_t)sess;
	response_pkt_p pkt = NULL;
	unsigned char ucheck;
	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "$auth,%s,%s{\"rslt\":\"ok\",\"seqno\":\"%s\"}", "monitor", sess->sname, seqno);
	ucheck = checksum((unsigned char*)buffer, cnt);
	cnt += sprintf_s(buffer + cnt, sizeof(buffer) - cnt, "*%02X\r\n", ucheck);
	pkt = response_pkt_new(client()->pktpool, cnt);
	memcpy(pkt->data, buffer, cnt);
	pkt->len = cnt;
	pkt->data[pkt->len] = '\0';
	p2p_stream_push(p2p, pkt);

	xhash_put(client()->sessions, sess->sname, sess);

	log_write(p2p->sess.client->log, LOG_NOTICE, "send auth ack to %s:%s\n", sess->sname, buffer);

}

static void sub_hook(sess_t sess, char *jstr)
{
	cJSON *jsonroot, *json;
	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL)
		return;
	char *task, *taskid;

	json = cJSON_GetObjectItem(jsonroot, "key");
	if (json == NULL || json->valuestring == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	task = _strdup(json->valuestring);	// 堆内存上分配空间

	taskid = add_taskid_to_queue(task);

	// step1. 将该taskid放入sess对应的任务队列sess->subs中
	add_subs_to_sess(sess, taskid);

	// step2. 将该sess放入tasikd对应的连接队列中
	add_sess_to_subtable(g_xsubbytaskid, taskid, sess);
	cJSON_Delete(jsonroot);
}

static void unsub_hook(sess_t sess, char *jstr)
{
	cJSON *jsonroot, *json;
	jsonroot = cJSON_Parse(jstr);
	if (jsonroot == NULL)
		return;
	char *taskid;
	sess_t s;

	json = cJSON_GetObjectItem(jsonroot, "key");
	if (json == NULL) {
		cJSON_Delete(jsonroot);
		return;
	}
	taskid = json->valuestring;

	// step1. 将该taskid从sess对应的任务队列sess->subs中移除
	del_subs_from_sess(sess, taskid);

	// step2. 将该sess从tasikd对应的连接队列中移除
	del_sess_from_subtable(g_xsubbytaskid, taskid, sess);

	cJSON_Delete(jsonroot);
}

static void forward_ack_to_frontsvr(char *head, char *jstr)
{
	char buffer[2048];
	int count = 0;
	int endpos = 0;
	response_pkt_p pkt;

	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$%s,%s,%s,%s", head,"monitor", "frontsvr", jstr);
	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
	endpos = count;
	buffer[endpos + 1] = '\0';

	if (client()->p2p_frontsvr != NULL)
	{
		pkt = response_pkt_new(((p2pclient_t)(client()->p2p_frontsvr))->pktpool, endpos + 1);
		memcpy(pkt->data, buffer, endpos + 1);
		pkt->len = endpos + 1;
		p2pclient_write((p2pclient_t)(client()->p2p_frontsvr), pkt);

		log_write(client()->log, LOG_NOTICE, "send %s to frontsvr:%s\n", head,buffer);
	}
}

//static void forward_taskpau_ack_to_frontsvr(char *jstr)
//{
//	char buffer[2048];
//	int count = 0;
//	int endpos = 0;
//	response_pkt_p pkt;
//
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$taskpau,%s,%s,%s", "monitor", "frontsvr", jstr);
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
//	endpos = count;
//	buffer[endpos + 1] = '\0';
//
//	if (client()->p2p_frontsvr != NULL)
//	{
//		pkt = response_pkt_new(((p2pclient_t)(client()->p2p_frontsvr))->pktpool, endpos + 1);
//		memcpy(pkt->data, buffer, endpos + 1);
//		pkt->len = endpos + 1;
//		p2pclient_write((p2pclient_t)(client()->p2p_frontsvr), pkt);
//
//		log_write(client()->log, LOG_NOTICE, "send taskpau ack to frontsvr:%s\n", buffer);
//	}
//}
//
//static void forward_taskcon_ack_to_frontsvr(char *jstr)
//{
//	char buffer[2048];
//	int count = 0;
//	int endpos = 0;
//	response_pkt_p pkt;
//
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$taskcon,%s,%s,%s", "monitor", "frontsvr", jstr);
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
//	endpos = count;
//	buffer[endpos + 1] = '\0';
//
//	if (client()->p2p_frontsvr != NULL)
//	{
//		pkt = response_pkt_new(((p2pclient_t)(client()->p2p_frontsvr))->pktpool, endpos + 1);
//		memcpy(pkt->data, buffer, endpos + 1);
//		pkt->len = endpos + 1;
//		p2pclient_write((p2pclient_t)(client()->p2p_frontsvr), pkt);
//
//		log_write(client()->log, LOG_NOTICE, "send taskcon ack to frontsvr:%s\n", buffer);
//	}
//}
//
//static void forward_taskdone_ack_to_frontsvr(char *jstr)
//{
//	char buffer[2048];
//	int count = 0;
//	int endpos = 0;
//	response_pkt_p pkt;
//
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$taskdone,%s,%s,%s", "monitor", "frontsvr", jstr);
//	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
//	endpos = count;
//	buffer[endpos + 1] = '\0';
//
//	if (client()->p2p_frontsvr != NULL)
//	{
//		pkt = response_pkt_new(((p2pclient_t)(client()->p2p_frontsvr))->pktpool, endpos + 1);
//		memcpy(pkt->data, buffer, endpos + 1);
//		pkt->len = endpos + 1;
//		p2pclient_write((p2pclient_t)(client()->p2p_frontsvr), pkt);
//
//		log_write(client()->log, LOG_NOTICE, "send taskdone ack to frontsvr:%s\n", buffer);
//	}
//}

//回复给前端：监控服务器
void  moniclient_exp_cb(int msgid, void* msg, int len, void* param)
{
	sess_t sess;
	p2p_t  p2p;
	int  msgtype;
	int  msgcode = 0;
	unsigned char* frame = (unsigned char*)msg;
	moniclient_exp_t exp = (moniclient_exp_t)param;
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
		//protobuffer_send_p2p(p2p, eProto_pong, sess->sname);
		//log_write(p2p->sess.client->log, LOG_NOTICE, "send pong to moniclient.");
	}
	break;

	case cunpack_auth:
	{
		log_write(client()->log, LOG_NOTICE, "recv auth req from %s.", sess->sname);
		auth_hook(sess,jstr);
	}
	break;

	case cunpack_sub:
	{
		// 注1：每个连接都维护了一个队列sess->subs，其中放置了该连接订阅的所有taskid
		// 注2：client->xsubbytaskid表中，以各taskid为主键，值指向了所有订阅了该taskid的连接组成的队列	
		log_write(client()->log, LOG_NOTICE, "recv sub from %s:%s\n", sess->sname, jstr);
		sub_hook(sess,jstr);
	}
	break;

	case cunpack_unsub:
	{
		// 注1：每个连接都维护了一个队列sess->subs，其中放置了该连接订阅的所有taskid
		// 注2：client->xsubbytaskid表中，以各taskid为主键，值指向了所有订阅了该taskid的连接组成的队列	
		log_write(client()->log, LOG_NOTICE, "recv unsub from %s:%s\n", sess->sname, jstr);
		unsub_hook(sess, jstr);
	}
	break;

	case cunpack_taskpau:
	{
		log_write(client()->log, LOG_NOTICE, "recv taskpau ack from %s.", sess->sname);
		//forward_taskpau_ack_to_frontsvr(jstr);
		forward_ack_to_frontsvr("taskpau", jstr);
	}
	break;

	case cunpack_taskcon:
	{
		log_write(client()->log, LOG_NOTICE, "recv taskcon ack from %s.", sess->sname);
		//forward_taskcon_ack_to_frontsvr(jstr);

		forward_ack_to_frontsvr("taskcon", jstr);
	}
	break;

	case cunpack_taskdone:
	{
		log_write(client()->log, LOG_NOTICE, "recv taskdone ack from %s.", sess->sname);
		//forward_taskcon_ack_to_frontsvr(jstr);

		forward_ack_to_frontsvr("taskdone", jstr);
	}
	break;

	case cunpack_verify:
	{
		log_write(client()->log, LOG_NOTICE, "recv verify ack from %s.", sess->sname);

		forward_ack_to_frontsvr("verify", jstr);
	}
	break;

	case cunpack_config:
	{
		log_write(client()->log, LOG_NOTICE, "recv config from %s.", sess->sname);

		forward_ack_to_frontsvr("config", jstr);
	}
	break;

	case cunpack_remoteunlock:
	{
		log_write(client()->log, LOG_NOTICE, "recv config from %s.", sess->sname);

		forward_ack_to_frontsvr("remoteunlock", jstr);
	}
	break;

	case cunpack_rptctrl:
	{
		log_write(client()->log, LOG_NOTICE, "recv config from %s.", sess->sname);

		forward_ack_to_frontsvr("rptctrl", jstr);
	}
	break;

	default:
		break;
	}
}
