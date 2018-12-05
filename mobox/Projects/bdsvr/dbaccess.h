#ifndef __DBACCESS_H__
#define __DBACCESS_H__

#include "myquery/mysqldb.h"
#include "grouprslt.h"
#include "CirQueue.h"

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct _sms2bdms_st
	{
		char sendtime[64];
		char delivertime[64];	//投递时间
		int  recvid;			//接收方北斗ID
		char sendphone[15];		//发送方手机号码 86开头
		int  icodetype;			//2:sms2bdmail  1:sms2bdmsg
		int iflag;				//发送成功为1（收到对方的回执），此时填delivertime字段
		char content[1024];		//通信内容
		int seqno;				//消息序列号
	}sms2bdms_st, *sms2bdms_t;

	typedef struct _bd2sms_st
	{
		char sendtime[64];
		char delivertime[64];	//投递时间
		int  sendbid;			//发送方北斗ID
		char recvphone[15];		//接收方手机号码 86开头
		int  icodetype;			//  1:bd2sms
		int  iflag;				//发送成功为1（收到对方的回执），此时填delivertime字段
		char content[1024];		//通信内容
		int  seqno;				//消息序列号
	}bd2sms_st, *bd2sms_t;

	typedef struct _sudp_message_st
	{
		char sendtime[64];
		char delivertime[64];	//投递时间
		int  isendid;			//发送方平台id
		char recvid[12];		//接收方平台Id
		int  icomtype;			//1-inet2bdmsg, 2-inet2bdmail, 3-inet2inetmsg，4-bd2inetmsg
		int iflag;				//发送成功为1（收到对方的回执），此时填delivertime字段
		char content[1024];		//通信内容
		int seqno;				//消息序列号
	}sudp_message_st, *sudp_message_t;

	typedef struct _bdsos_st
	{
		char postime[64];
		char sendtime[64];		//推送时间
		int  isendid;			//发送方平台id
		char content[1024];		//通信内容
		int seqno;				//消息序列号
	}bdsos_st, *bdsos_t;


	typedef Cir_Queue_t Rslt_GroupUsers_t;
	typedef Cir_Queue_st Rslt_GroupUsers_st;


	int db_get_hid_taskid_executor_by_bdid(mysqlquery_t dbinst, char * bdid, char**taskid, char**hid, char **executor);
int db_get_hid_bybdid(mysqlquery_t dbinst, char * bdid, char **hid);
int db_get_taskid_byhid(mysqlquery_t dbinst, int hid, char **taskid);

#ifdef __cplusplus
}
#endif

#endif