#ifndef __MESSAHE_H__
#define __MESSAHE_H__
#include <string>
#include "type.h"
typedef int BOOL;

using namespace std;

//账户表格
struct account_st 
{
	string username; //手机号
	string pwd;
	string strname; //昵称
	int    iage;//年龄
	string strworkunit;// 工作单位
	string strmail;//邮箱
};

//订单表格
struct orderform_st{
	string strdate;  //日期 YYYY-MM-DD
	int isession;   //场次
	string strusername;  //用户名:手机号
	string strname; //昵称
	int    iage;//年龄
	string strworkunit;// 工作单位
	string strmail;//邮箱
	int iordercode;//订票个数
	string strordernumber; //订单号
	string strIdentify;//验证码
	int  iordercount;//定了几张票
	int iordertype; //订单类型
	int ibuse;   //是否已使用
	string strordertime;//订票时间
	int payment;//是否已支付 0未支付  1已支付
};

//用户体验成绩
struct userscore_st
{
	string strdate;  //日期 YYYY-MM-DD
	int isession;   //场次
	string strusername;  //用户名:手机号
	string strname; //昵称
	int    iage;//年龄
	string strworkunit;// 工作单位
	string strmail;//邮箱
	string strordernumber; //订单号
	string score;
	string travescence; //玩了哪些场景
	
};
//票资源
struct ticketresource_st{
	string strdate;  //日期 YYYY-MM-DD
	int isession;

	int iticketsum;//
	int iticketremain;
	string strstarttime; //起始时间
	string strendtime; //结束时间
	int    iavaility;
};

//场次<废弃使用>
struct sessioninfo_st 
{
	string strdate;
	int    isession;
	string strstarttime;
	string strendtime;
	int    iavaility;
};

//绑定
struct bindcard_st 
{	
	int icardid;
	string strusername; //订单号
	string strmaketime;//制卡时间
};

struct loguser_st
{
	string username;
	string pwd;
};

//游客体验顺序,制卡时间
struct playnumber_st
{
	int id;
	string strordernumber;
	string strmaketime;//制卡时间
};

#endif