#ifndef __USER_STATUS_H__
#define __USER_STATUS_H__

#define _status_offline  0
#define _status_online   1
#define _status_bad      2
#define _user_inet       0
#define _user_bd         1

//���������
//#define _max_life        20
//#define _max_life      (atoi(client()->plat_conf->warntime) * 60)
#define  _max_life		(g_warntime)

#include "util/util.h"


typedef struct _user_status_arg_st
{
	char executor[64];
	char lon[30];
	char lat[30];
	char hgt[30];
}user_status_arg_st, *user_status_arg_t;


typedef struct _user_status_st{
	char key[64];//�û���ֵ
	int  status;//reserved
	int  life;//sec
	int  type;
	user_status_arg_st arg;
}user_status_st,*user_status_t;

user_status_t user_status_clone(user_status_t us);

typedef struct _users_act_st{
	xht      _t;
	jqueue_t _q;
}users_act_st, *users_act_t;

//�½�����
users_act_t    users_act_new(int prime);

//�ͷŲ���
void           users_act_free(users_act_t us);

//�����û���״̬
int            users_act_set(users_act_t us, user_status_t stat);

//��ѯkeyֵָ�����û���״̬��
user_status_t  users_act_get(users_act_t us, char* key);

//��ָ��key ��userɾ����ɾ���󣬳������¼��룬�����ѯ�����û���
void           users_act_del(users_act_t us, char* key);

//��ָ��key��user���¼���ӳ������������ֵ.
void           users_act_save(users_act_t us, char* key, int sec, int type,void *arg);

//�ּ�������users.
jqueue_t       users_act_check(users_act_t us, int sec);

#endif