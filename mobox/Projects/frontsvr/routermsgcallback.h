#ifndef _DEV_MSG_CALLBACK__H__
#define _DEV_MSG_CALLBACK__H__
#include <time.h>
#define MAX_INETMSG_NUM  100
#define MAX_ONLINE_USER  1000
#define MAX_ERROR_NUM  3

typedef enum {
	cunpack_active = 200,
	cunpack_inetmsg,
	cunpack_bdmail,
	cunpack_sos,
	cunpack_pos,
	cunpack_bdposrpt,
	cunpack_delete,
}keytype;

//缓存Inet消息
typedef struct _inetmsg_st
{
	int pid; //平台Id
	int seqno; //消息序列号
	int inum;//消息重复发送的次数
}inetmsg_st,*inetmsg_t;

typedef struct _inetpresence_st
{
	int pid; //平台Id
	time_t lastsendtime;
}inetpresence_st, *inetpresence_t;
static int g_inetmsgcount = 0;
static int g_peresencecount = 0;
static inetmsg_st g_InetMsg[MAX_INETMSG_NUM];
static inetpresence_st g_inetperesence[MAX_ONLINE_USER];

time_t GetLatSendtime(int pid);
void   InsertPresence(int pid, time_t sendtime);
void   ReSortMsg(int idel);
void   DeleteMsg(int ipid, int iseqno);
void   HandleErrorInetMsg(int pid, int iseqno);//将有问题的互联网消息放到
void   router_exp_cb(int msgid, void* msg, int itype, int pid, int len, void* param);
void   process_inet_smcache(int pid);  //处理缓存中发往互联网的数据
void   process_bd_smcache(int pid);    //处理缓存中发往北斗的数据
int    ispresence(int pid,int inet); //inet = 1  inetactivetime;  inet = 0 bdactivetime
#endif