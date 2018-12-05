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
		char delivertime[64];	//Ͷ��ʱ��
		int  recvid;			//���շ�����ID
		char sendphone[15];		//���ͷ��ֻ����� 86��ͷ
		int  icodetype;			//2:sms2bdmail  1:sms2bdmsg
		int iflag;				//���ͳɹ�Ϊ1���յ��Է��Ļ�ִ������ʱ��delivertime�ֶ�
		char content[1024];		//ͨ������
		int seqno;				//��Ϣ���к�
	}sms2bdms_st, *sms2bdms_t;

	typedef struct _bd2sms_st
	{
		char sendtime[64];
		char delivertime[64];	//Ͷ��ʱ��
		int  sendbid;			//���ͷ�����ID
		char recvphone[15];		//���շ��ֻ����� 86��ͷ
		int  icodetype;			//  1:bd2sms
		int  iflag;				//���ͳɹ�Ϊ1���յ��Է��Ļ�ִ������ʱ��delivertime�ֶ�
		char content[1024];		//ͨ������
		int  seqno;				//��Ϣ���к�
	}bd2sms_st, *bd2sms_t;

	typedef struct _sudp_message_st
	{
		char sendtime[64];
		char delivertime[64];	//Ͷ��ʱ��
		int  isendid;			//���ͷ�ƽ̨id
		char recvid[12];		//���շ�ƽ̨Id
		int  icomtype;			//1-inet2bdmsg, 2-inet2bdmail, 3-inet2inetmsg��4-bd2inetmsg
		int iflag;				//���ͳɹ�Ϊ1���յ��Է��Ļ�ִ������ʱ��delivertime�ֶ�
		char content[1024];		//ͨ������
		int seqno;				//��Ϣ���к�
	}sudp_message_st, *sudp_message_t;

	typedef struct _bdsos_st
	{
		char postime[64];
		char sendtime[64];		//����ʱ��
		int  isendid;			//���ͷ�ƽ̨id
		char content[1024];		//ͨ������
		int seqno;				//��Ϣ���к�
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