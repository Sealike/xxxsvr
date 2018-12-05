#ifndef __USER_STATUS_H__
#define __USER_STATUS_H__

#define _status_offline  0
#define _status_online   1
#define _status_bad      2
#define _user_inet       0
#define _user_bd         1

//最大生存期
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
	char key[64];//用户键值
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

//新建操作
users_act_t    users_act_new(int prime);

//释放操作
void           users_act_free(users_act_t us);

//设置用户的状态
int            users_act_set(users_act_t us, user_status_t stat);

//查询key值指定的用户的状态。
user_status_t  users_act_get(users_act_t us, char* key);

//将指定key 的user删除，删除后，除非重新加入，否则查询不到用户。
void           users_act_del(users_act_t us, char* key);

//将指定key的user重新激活，延长其寿命至最大值.
void           users_act_save(users_act_t us, char* key, int sec, int type,void *arg);

//分拣出不活动的users.
jqueue_t       users_act_check(users_act_t us, int sec);

#endif