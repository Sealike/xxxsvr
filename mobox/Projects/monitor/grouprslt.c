#include "grouprslt.h"
#include "stdlib.h"
#include <string.h>
#include <Windows.h>

userinfo_t userinfo_new()
{
	userinfo_t rtn = NULL;

	while ((rtn = (userinfo_t)calloc(1, sizeof(userinfo_st))) == NULL) Sleep(10);

	return rtn;
}

void  userinfo_free(userinfo_t info)
{
	if(info){
		free(info);
	}
}

userinfo_t  userinfo_assign(userinfo_t info, int id)
{
	if(info)
	{
		info->id = id;
	}

	return info;
}
static void _assure_sos_capacity(sos_t con){
	if (con == NULL) return;

	if (con->iusercount == con->capcity){
		con->infos = (userinfo_t*)realloc(con->infos, (con->iusercount + con->iusercount / 2)*sizeof(userinfo_t));
		con->capcity = (con->iusercount + con->iusercount / 2);
	}
}

static void _assure_pos_capacity(pos_t con)
{
	if (con == NULL) return;

	if (con->iusercount == con->capcity){
		con->infos = (userinfo_t*)realloc(con->infos, (con->iusercount + con->iusercount / 2)*sizeof(userinfo_t));
		con->capcity = (con->iusercount + con->iusercount / 2);
	}
}

static void _assure_capacity(groupinfos_t con)
{
	if(con == NULL) return;

	if(con->iusercount == con->capcity){
		con->infos = (userinfo_t*)realloc(con->infos, (con->iusercount + con->iusercount / 2)*sizeof(userinfo_t));
		con->capcity = (con->iusercount + con->iusercount / 2);
	}
}

groupinfos_t groupinfos_new()
{
	groupinfos_t rtn = NULL;

	while ((rtn = (groupinfos_t)calloc(1, sizeof(groupinfos_st))) == NULL) Sleep(10);

	rtn->capcity = _init_userinfo_capacity;
	rtn->infos = (userinfo_t*)calloc(sizeof(userinfo_st), _init_userinfo_capacity);
	rtn->iusercount = 0;

	return rtn;
}

int  groupinfos_add(groupinfos_t con, userinfo_t info)
{
	if(con == NULL || info == NULL) return 0;

	_assure_capacity(con);

	con->infos[con->iusercount++] = info;

	return con->iusercount;
}

void groupinfos_clear(groupinfos_t con)
{
	if(con == NULL ) return ;

	con->iusercount = 0;
}

void groupinfos_free(groupinfos_t con)
{
	if(con == NULL) return;

	if(con->infos){
		int i = 0;
		for (; i<con->iusercount; i++)
			userinfo_free(con->infos[i]);
		free(con);
	}

}

int  groupinfos_num(groupinfos_t con)
{
	if (con) return con->iusercount;

	return 0;
}

sos_t sos_new()
{
	sos_t rtn = NULL;

	while ((rtn = (sos_t)calloc(1, sizeof(sos_st))) == NULL) Sleep(10);

	rtn->capcity = _init_userinfo_capacity;
	rtn->infos = (userinfo_t*)calloc(sizeof(userinfo_st), _init_userinfo_capacity);
	rtn->iusercount = 0;

	return rtn;
}
int sos_add(sos_t con, userinfo_t info)
{
	if (con == NULL || info == NULL) return 0;

	_assure_sos_capacity(con);

	con->infos[con->iusercount++] = info;

	return con->iusercount;
}
void sos_clear(sos_t con)
{
	if (con == NULL) return;

	con->iusercount = 0;
}
void sos_free(sos_t con)
{
	if (con == NULL) return;

	if (con->infos){
		int i = 0;
		for (; i < con->iusercount; i++)
			userinfo_free(con->infos[i]);
		free(con);
	}
}
int sos_num(sos_t con)
{
	if (con) return con->iusercount;

	return 0;
}

pos_t pos_new()
{
	pos_t rtn = NULL;

	while ((rtn = (pos_t)calloc(1, sizeof(pos_st))) == NULL) Sleep(10);

	rtn->capcity = _init_userinfo_capacity;
	rtn->infos = (userinfo_t*)calloc(sizeof(userinfo_st), _init_userinfo_capacity);
	rtn->iusercount = 0;

	return rtn;
}
int pos_add(pos_t con, userinfo_t info)
{
	if (con == NULL || info == NULL) return 0;

	_assure_pos_capacity(con);

	con->infos[con->iusercount++] = info;

	return con->iusercount;
}
void pos_clear(pos_t con)
{
	if (con == NULL) return;

	con->iusercount = 0;
}
void pos_free(pos_t con)
{
	if (con == NULL) return;

	if (con->infos){
		int i = 0;
		for (; i < con->iusercount; i++)
			userinfo_free(con->infos[i]);
		free(con);
	}
}

int pos_num(pos_t con)
{
	if (con) return con->iusercount;
	return 0;
}

usertable_t usertable_new()
{
	usertable_t rtn = NULL;

	while ((rtn = (usertable_t)calloc(1, sizeof(usertable_st))) == NULL) Sleep(10);
	return rtn;
}
void usertable_free(usertable_t info)
{
	if (info){
		free(info->name);
		free(info->phoneno);
		free(info->mailbox);
		free(info);
	}
}

account_t account_new()
{
	account_t rtn = NULL;

	while ((rtn = (account_t)calloc(1, sizeof(account_st))) == NULL) Sleep(10);
	return rtn;
}
void account_free(account_t info)
{
	if (info){
		free(info->name);
		free(info->pwd);
		free(info->code);
		free(info);
	}
}


void user_free(user_t info)
{
	if (info){
		if (info->name)
			free(info->name);
		if (info->phoneno)
			free(info->phoneno);
		if(info->countycode)
			free(info->countycode);
		if(info->towncode)
			free(info->towncode);
		if(info->countrycode)
			free(info->countrycode);
		if(info->groupcode)
			free(info->groupcode);
	}
}