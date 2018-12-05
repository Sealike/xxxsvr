// smudpsvr.cpp : 定义控制台应用程序的入口点。
//
#include "bdexplain.h"
#include "stdafx.h"
#include "instapp.h"
#include "p2p.h"
#include "p2pclient.h"
#include "expat_bdsvr.h"
#include "protobuffer.h"
#include "global.h"
#include "BDDeviceArray.h"
#include "bdsvrcallback.h"

#define  _PROTOCAL_4_0   ("bdprotocal4_0")
#define  _PROTOCAL_2_1   ("bdprotocal2_1")
#define __remote_hasjob 1
#define __remote_nojob  0
int g_shutdown = 0;

static BOOL check_zombie(sess_t sess, time_t now){
	BOOL rtn = FALSE;

	if ((now - sess->last_activity) > 7){
		rtn = TRUE;
	}

	return rtn;
}

static void remove_zombie_from_lives()
{	
	int  i = 0;
	int  count = jqueue_size(g_instapp->session_que);
	time_t now = time(0);

	for(;i<count;i++)
	{
		sess_t sess = (sess_t)jqueue_pull(g_instapp->session_que);

		if(sess == NULL) continue;

		if (check_zombie(sess, now))
			jqueue_push(g_instapp->zombie_que,sess,0);//p2pclient_close((p2pclient_t)sess);
		else
			jqueue_push(g_instapp->session_que,sess,0);
	}
}

static void clear_zombies()
{
	int  i = 0;
	int  count = jqueue_size(g_instapp->session_que);

	for (; i < count; i++)
	{
		sess_t sess = (sess_t)jqueue_pull(g_instapp->zombie_que);

		if (sess == NULL) continue;

		if (sess->type == stream_SESS)
		   p2pclient_close((p2pclient_t)sess);
		
	}
}

static void check_dead_from_lives(sess_t sessdead)
{
	int i = 0;
	int count = jqueue_size(g_instapp->session_que);

	for(;i < count ; i++){
		void* p = jqueue_pull(g_instapp->session_que);
		if(sessdead == p) break;

		jqueue_push(g_instapp->session_que,p,0);
	}
}
static void clear_deads_from_lives()
{
	int count = jqueue_size(g_instapp->dead_sess);
	int i = 0;

	for(;i<count;i++){
		sess_t sess = (sess_t)jqueue_pull(g_instapp->dead_sess);
		jqueue_push(g_instapp->dead_sess,sess,0);
		check_dead_from_lives(sess);
	}
}
static void _save_deadsession(int waits)
{
	int count = 0;
	int i = 0;
	time_t now = time(0);
	static time_t last = 0;

	if(last == 0) last = now;

	if(now - last < waits) return ;

	last = now;

	count = jqueue_size(g_instapp->dead_sess);

	for(;i < count ;i++)
	{
		sess_t dead = (sess_t)jqueue_pull(g_instapp->dead_sess);		
		p2pclient_t p2p = (p2pclient_t)dead;
		BOOL connected = p2pclient_connect(p2p,g_instapp->mio,g_instapp->dead_sess,g_instapp,stream_SESS,NULL,g_instapp);		
	}
}

//Ping协议
static void _pingsession(int waits)
{
	int i = 0;
	int count = 0;
	time_t now = time(0);
	static time_t last = 0;
	if(last == 0) last = now;

	if(now - last < waits) return ;

	last = now;

	count = jqueue_size(g_instapp->session_que);

	for(;i<count;i++)
	{
		sess_t sess = (sess_t)jqueue_pull(g_instapp->session_que);
		jqueue_push(g_instapp->session_que, sess, 0);
		if(sess->binitsess == TRUE)
		{
			p2pclient_t  p2p  = (p2pclient_t)sess;
			protobuffer_send(p2p,eProto_ping,NULL);
			log_write(g_instapp->log, LOG_NOTICE, "ping %s", sess->sname);
		}
	}
}



/*注释*/
static void _app_config_expand(instapp_t instapp) 
{	
	char * strip = NULL;
	strip  = config_get_one(instapp->config, "bdsvr.ip", 0);
	if(strip == NULL)
		instapp->bdsvr_ip = _strdup("127.0.0.1");
	else
		instapp->bdsvr_ip = _strdup(strip);
	instapp->bdsvr_port = j_atoi(config_get_one(instapp->config, "bdsvr.port", 0), 0);
	if (instapp->bdsvr_port == 0) instapp->bdsvr_port = 3348;

	instapp->p2pbdsvr = _strdup("");

	//得到北斗终端设备
	instapp->idecvicecount = j_atoi(config_get_one(instapp->config, "device.number", 0), 1); //
	instapp->ihascache = j_atoi(config_get_one(instapp->config, "device.cache", 0), 0); 
	for (int i = 0; i < instapp->idecvicecount; i++)
	{
		char ckey[24] = { 0 };
		sprintf(ckey, "cominfo%d.comnum", i);
		instapp->deviceinfo[i].icomport= j_atoi(config_get_one(instapp->config, ckey, 0), 1); //串口号
		sprintf(ckey, "cominfo%d.protocal", i);
		instapp->deviceinfo[i].iprotocal = j_atoi(config_get_one(instapp->config, ckey, 0), 1); //协议
		sprintf(ckey, "cominfo%d.baud", i);
		instapp->deviceinfo[i].ibaud = j_atoi(config_get_one(instapp->config, ckey, 0), 0); //波特率
		if (instapp->deviceinfo[i].ibaud == 0)
		{
			instapp->deviceinfo[i].ibaud = 115200;
		}
	}

	
}

static unsigned char checksum(unsigned char buffer[], int len)
{
	unsigned char data = 0;
	int i = 0;
	for (; i < len; i++)
		data ^= buffer[i];

	return data;
}

int _tmain(int argc, _TCHAR* argv[])
{
	/*char buffer[1024] = "54 58 53 51 00 1d 02 20 30 46 06 f5 07 00 58 00 a4 bf 26 00 00 72 00 00 c7 eb b2 bb d2 aa c2 d2 bf b3 2a";
	unsigned char data = 0;
	data = checksum((unsigned char*)buffer, sizeof(buffer));
	printf("data = %d", data);
	while (1);*/

	p2pclient_t  p2p = 0;
	p2pclient_t  p2bdsvr = 0;
	const char* config_file = "../../etc/bdjob.xml";

	g_instapp = instapp_new();
	g_instapp->log = log_new(log_STDOUT,NULL,NULL);
	g_instapp->pktpool = response_pkt_pool_new();
	g_instapp->config  = config_new();

	if(config_load(g_instapp->config, config_file) != 0)
	{
		fputs("sensor: couldn't load config, aborting\n", stderr);
		instapp_free(g_instapp);
		return 2;
	}

	_app_config_expand(g_instapp);

	g_instapp->stream_maxq = 1024;	
	g_instapp->sessions    = xhash_new(1023);
	g_instapp->session_que = jqueue_new();
	g_instapp->zombie_que = jqueue_new();
	g_instapp->dead_sess   = jqueue_new();
	g_instapp->mio = mio_new(FD_SETSIZE);
	//bdsvr服务器
	p2p = p2pclient_new(g_instapp->bdsvr_ip,g_instapp->bdsvr_port,stream_SESS); //tcp连接
	if (!p2pclient_connect(p2p, g_instapp->mio, g_instapp->dead_sess, g_instapp, stream_SESS, NULL, g_instapp)){
		printf("connect to  bdsvr failed ,exit..\n");
		instapp_free(g_instapp);
		return 1;
	}else{
		p2p->expat = (_exp_t)bdsvr_exp_new(&p2p->sess); //回调函数
		exp_set_callback(p2p->expat, bdsvr_exp_cb);
	}
	g_instapp->p2pbdsvr = p2p;

	//北斗接收
 	protocalname_t pName = NULL;
	for (int inum = 0; inum < g_instapp->idecvicecount; inum++)
	{
		CDevice *pBDMessage = new CDevice;
		if (g_instapp->deviceinfo[inum].iprotocal == 0)
		{
			pName = _PROTOCAL_4_0;
		}
		else
		{
			pName = _PROTOCAL_2_1;
		}
		bool bComSuccess = false;;
		bool bSuccess = pBDMessage->Open(pName, g_instapp->deviceinfo[inum].icomport, g_instapp->deviceinfo[inum].ibaud, 8, 0, FALSE, 0, "", NULL, bComSuccess);
		if (bSuccess == false || bComSuccess == false)
		{
			log_write(g_instapp->log, LOG_ERR, "connect COM%d failed.", g_instapp->deviceinfo[inum].icomport);
			free(pBDMessage); //释放
		}
		else 
		{
			log_write(g_instapp->log, LOG_NOTICE, "connect COM%d success.", g_instapp->deviceinfo[inum].icomport);
			pBDMessage->InitDevice(); //发送IC检测
			g_Remoteinstance.GetBDArray()->InsertWorkings(pBDMessage);
		}
	}
	CBDDeviceArray *pBDDeviceArray = CBDDeviceArray::Get();
	if (g_instapp->idecvicecount > 0)
	{
		pBDDeviceArray->InitThread();
		g_Remoteinstance.SetRemoteAgentStatus(__remote_hasjob);
	}

	while(!g_shutdown)
	{
		static time_t last = 0;
		time_t now = time(NULL);

		if(last == 0) last = now;

		mio_run(g_instapp->mio,1);

		clear_deads_from_lives();	

		_save_deadsession(7);

		if(now - last > 2){
			remove_zombie_from_lives();
			last = now;
			clear_zombies();
		}

		_pingsession(2);
		
		//发送工作请求
		if (((p2pclient_t)g_instapp->p2pbdsvr)->lost == 0)
		{
			char* strJobMessage = NULL;
			BOOL bFree = g_Remoteinstance.GetBDArray()->HasFreeDevice();
			if (g_Remoteinstance.GetRemoteAgentStatus() == __remote_hasjob && bFree == TRUE)			
				strJobMessage = "$JOBME*E1\r\n";
			
			if (strJobMessage)
			{
				response_pkt_p pkt;
				pkt = response_pkt_new(g_instapp->pktpool, strlen((const char*)strJobMessage));
				memcpy(pkt->data, strJobMessage, strlen((const char*)strJobMessage));
				pkt->len = strlen((const char*)strJobMessage);
				
				p2pclient_write((p2pclient_t)g_instapp->p2pbdsvr, pkt);//打包发送给bdsvr
				log_write(g_instapp->log, LOG_NOTICE, "send $JOBME.");
			}
		}
	}

	instapp_free(g_instapp);
	return 0;
}

