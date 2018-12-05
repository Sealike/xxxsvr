#include "httpclient.h"
#include "bdjobber_callback.h"
#include "findjobber.h"
#include "jobqueue.h"
#include "instapp.h"
#include "json/cJSON.h"
#include "p2p.h"

extern BOOL jobber_free;

extern void ParseEncodeSend(p2p_t p2p);
static  char* UTF8toGBK(char *pText)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, pText, -1, NULL, 0);
	unsigned short *wszgbk = malloc(sizeof(unsigned short)*(len + 1));
	memset(wszgbk, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, pText, -1, wszgbk, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszgbk, -1, NULL, 0, NULL, NULL);
	char *szgbk = malloc(len + 1);
	memset(szgbk, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszgbk, -1, szgbk, len, NULL, NULL);
	free(wszgbk);
	szgbk[len - 1] = '\0';
	return szgbk;
}
static void str_slice_put(char * getstr)
{
	cJSON *json_array, *jsonroot;
	int json_cnt;

	log_write(g_instapp->log, LOG_NOTICE, "recv response from http server,now slice and putting...");
	// 对一个json串集合进行解析，解出一个json串数组
	json_array = cJSON_Parse(getstr);
	if (json_array == NULL)
	{
		cJSON_Delete(json_array);
		return;
	}
	int isize = cJSON_GetArraySize(json_array);
	for (json_cnt = 0; json_cnt < isize; json_cnt++)
	{
		jsonroot = cJSON_GetArrayItem(json_array,json_cnt);	// 取出一条json串
		bdjob_t bdjob = bdjob_new();
		//bdjob->d = ++(g_instapp->bdjob_num);
		char* jsonstr = NULL;
		jsonstr = cJSON_Print(jsonroot);
		char *gbkcode = UTF8toGBK(jsonstr);
		memcpy(bdjob->content, gbkcode, strlen(gbkcode));
		log_write(g_instapp->log, LOG_NOTICE, "put %s into jqueue.", bdjob->content);
		//压入任务
		jobqueue_putjob(g_instapp->jobqueue, bdjob);
		free(jsonstr);
		free(gbkcode);
		//
	}
	cJSON_Delete(json_array);
}

static void *_work_fun(void *arg) {

	//sess_t destsess;
	//const char * cbdjobber = "bdjob";
	//destsess = getrefclnt(cbdjobber);
	//p2p_t p2p = (p2p_t)destsess;		// 当需要给bdjobber发消息时，需要这个p2p	
	char url[1024];
	char task_date[20];
	SYSTEMTIME T;


	httpclient_t httpclient = (httpclient_t)arg;
	int  i = 0;
	time_t now;

	log_debug(ZONE, "[%p] the worker thread has been running", pthread_self());
	while (!httpclient->shutdown)
	{	
 		DWORD dwCode = WaitForSingleObject(httpclient->evtstop, 5000);
		
		switch (dwCode){
		case WAIT_TIMEOUT :{}break;
		case WAIT_OBJECT_0:{}break;		
		default: break;
		}

		if (httpclient->shutdown) break;
		
		
		GetLocalTime(&T);
		sprintf(task_date, "%04d-%02d-%02d", T.wYear, T.wMonth, T.wDay);

		sprintf_s(url, sizeof(url), "http://%s:%d/%s?date=%s", g_instapp->https_ip, g_instapp->https_port, g_instapp->httpc_cmd, "2017-11-23");
		//sprintf_s(url, sizeof(url), "http://%s:%d/%s?date=%s", g_instapp->https_ip, g_instapp->https_port, g_instapp->httpc_cmd,task_date);
		
		log_debug(ZONE, "send http req %s to webserver.");

		char* getstr = httpc_doget(httpclient->httpc, url, 2000, 3000);
		if (getstr == NULL) {
			log_debug(ZONE, "[%p] open http doget failed (3000ms)", pthread_self());
			continue;
		}
		
		// getstr中内容逐条分离，并存入队列中
		str_slice_put(getstr);

		/* 该线程中只负责存储数据，不负责判断发送，所有的判断与发送都只在主线程的main函数的while中进行 */

	}
	log_debug(ZONE, "[%p] worker thread exit", pthread_self());
	return NULL;
}

httpclient_t httpclient_new(char* ip, int port)
{
	httpclient_t rtn = NULL;

	if (ip == NULL || port <= 0) return NULL;

	while ((rtn = calloc(1, sizeof(httpclient_st))) == NULL) Sleep(1);
	rtn->httpc = httpc_new();
	rtn->https_ip = _strdup(ip);
	rtn->https_port = port;
	rtn->evtstop = CreateEvent(NULL, FALSE, FALSE, NULL);

	return rtn;
}

void  httpclient_free(httpclient_t client)
{
	if (client == NULL) return;

	if (client->httpc) httpc_free(client->httpc);

	if (client->https_ip) free(client->https_ip);

	if (client->evtstop) CloseHandle(client->evtstop);

}

int  httpclient_start(httpclient_t client)
{
	int ret = 0;

	if (client == NULL || client->httpc == NULL || client->https_ip == NULL) return 0;
		
	client->shutdown = 0;

	ret = pthread_create(&client->worker, NULL, _work_fun, client);

	return ret;
}

void  httpclient_stop(httpclient_t client)
{
	client->shutdown = TRUE;

	SetEvent(client->evtstop);
}