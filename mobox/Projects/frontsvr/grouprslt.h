#ifndef _GROUP_RSLT_H__
#define _GROUP_RSLT_H__

typedef struct _userinfo_st{
	int  id;
}userinfo_st, *userinfo_t;

userinfo_t   userinfo_new();
void         userinfo_free(userinfo_t);
userinfo_t   userinfo_assign(userinfo_t info, int id);

#define _init_userinfo_capacity 256

//分组下的用户
typedef struct _groupinfos_st{
	userinfo_t* infos; //组员
	int  igid; //组id 
	int iusercount; //组员个数
	int capcity;
}groupinfos_st, *groupinfos_t;

groupinfos_t groupinfos_new();

int  groupinfos_add(groupinfos_t con, userinfo_t info);

void groupinfos_clear(groupinfos_t con);

void groupinfos_free(groupinfos_t con);

int  groupinfos_num(groupinfos_t con);

typedef struct _sos_st
{
	userinfo_t* infos; //组员
	int iusercount;
	int capcity;
}sos_st, *sos_t;
sos_t sos_new();
int sos_add(sos_t con, userinfo_t info);
void sos_clear(sos_t con);
void sos_free(sos_t con);
int sos_num(sos_t con);

typedef struct _pos_st
{
	userinfo_t* infos; //组员
	int iusercount;
	int capcity;
}pos_st, *pos_t;
pos_t pos_new();
int pos_add(pos_t con, userinfo_t info);
void pos_clear(pos_t con);
void pos_free(pos_t con);
int pos_num(pos_t con);

typedef struct _usertable_st
{
	int id; //平台标识
	char *name; //昵称
	char *phoneno; //手机号
	char *mailbox;//邮箱
	int bid; //北斗号
}usertable_st, *usertable_t;
usertable_t usertable_new();
void usertable_free(usertable_t con);


typedef struct _account_st
{
	char *name;
	char *pwd;
	int itype;//itype=0 :county县  itype=1：town镇  itype=2：country村
	char *code;//编码
}account_st, *account_t;

account_t account_new();
void account_free(account_t con);

typedef struct _user_st
{
	char name[64];   //昵称
	char phoneno[64];//手机号
	char countycode[64]; //县编码
	char towncode[64];   //乡镇编码
	char countrycode[64];//村编码
	char groupcode[64];  //组编码

	int bid; //北斗号
}user_st, *user_t;
void user_free(user_t info);

#endif