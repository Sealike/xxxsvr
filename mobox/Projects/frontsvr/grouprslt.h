#ifndef _GROUP_RSLT_H__
#define _GROUP_RSLT_H__

typedef struct _userinfo_st{
	int  id;
}userinfo_st, *userinfo_t;

userinfo_t   userinfo_new();
void         userinfo_free(userinfo_t);
userinfo_t   userinfo_assign(userinfo_t info, int id);

#define _init_userinfo_capacity 256

//�����µ��û�
typedef struct _groupinfos_st{
	userinfo_t* infos; //��Ա
	int  igid; //��id 
	int iusercount; //��Ա����
	int capcity;
}groupinfos_st, *groupinfos_t;

groupinfos_t groupinfos_new();

int  groupinfos_add(groupinfos_t con, userinfo_t info);

void groupinfos_clear(groupinfos_t con);

void groupinfos_free(groupinfos_t con);

int  groupinfos_num(groupinfos_t con);

typedef struct _sos_st
{
	userinfo_t* infos; //��Ա
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
	userinfo_t* infos; //��Ա
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
	int id; //ƽ̨��ʶ
	char *name; //�ǳ�
	char *phoneno; //�ֻ���
	char *mailbox;//����
	int bid; //������
}usertable_st, *usertable_t;
usertable_t usertable_new();
void usertable_free(usertable_t con);


typedef struct _account_st
{
	char *name;
	char *pwd;
	int itype;//itype=0 :county��  itype=1��town��  itype=2��country��
	char *code;//����
}account_st, *account_t;

account_t account_new();
void account_free(account_t con);

typedef struct _user_st
{
	char name[64];   //�ǳ�
	char phoneno[64];//�ֻ���
	char countycode[64]; //�ر���
	char towncode[64];   //�������
	char countrycode[64];//�����
	char groupcode[64];  //�����

	int bid; //������
}user_st, *user_t;
void user_free(user_t info);

#endif