#pragma once
#include "messagess.h"
#include <vector>
#include "DbQuery.h"
#include "IniWrap.h"

using namespace std;


//账户信息
class CAccountBox{
public:
	BOOL  Insert(account_st &account);
	void  Query(string username,account_st &account); //查询某个用户的信息
	void  Query(vector<account_st> &out);//查询所有用户信息
	BOOL  Update(string username,account_st &account);//修改某个用户的信息
	int   Delete(string username);
	int   Clear();//删除所有用户
};

//订单表格
class COrderFormBox
{
	public:
	BOOL Insert(orderform_st &orderform);
	void Query(string ordernumber,string ordercode,orderform_st &orderform);//查询某条订单信息
	void Query(string ordernumber,orderform_st &orderform);//
	void Query(vector<orderform_st> &out);//查询所有订单
	void Query_by_phone(string phone,vector<orderform_st> &out);//根据手机号查询
	BOOL Update(string ordernumber,orderform_st &orderform);//修改某条订单信息
	BOOL Update(string ordernumber,int payment);//支付
	BOOL Update_use(string ordernumber,int ibuse);//是否已使用
	int  Delete(string ordernumber);
	int  Clear();
};

//票资源
class CTicketResource
{
	public:
	BOOL  Insert(ticketresource_st &ticketinfo);
	void  Query(string strdate,vector<ticketresource_st> &ticketinfo);
	void  Query_span(string startdate,string enddate,vector<ticketresource_st> &ticketinfo);
	void  Query(string strdate,int isession,int &iSum,int &iRemain);//查询票数
	void  Query(string strdate,int isession,string &strStartTime,string &strEndTime);
	void  Query(vector<ticketresource_st> &out);//
	void  Query_Session(string startdate,string strStartTime,string strEndTime,int &isession);//查询某个时间是否已经被设置为特定场次
	BOOL  Update(string strdate,int isession,ticketresource_st &ticketinfo);//
	BOOL  Update(string strdate,int isession,int iuse);//
	int   Decrease(string strdate,int isession,int icount);//减少n张票
	int   Increase(string strdate,int isession,int icount);//增加n张票
	int   Delete(string strdate,int isession);
	int   Delete(string strdate);
	int   Delete(string startdate,string enddate);
	int   Clear();


};


//场次
//class CSessionInfo
//{
//	public:
//	BOOL    Insert(sessioninfo_st &sessioninfo);
//	void    Query(string strdate,vector<sessioninfo_st> &out);
//	void    Query(string strdate,int isession,sessioninfo_st &out);
//	BOOL    Update(string strdate,int isession,sessioninfo_st &out);
//	void    SetEnable(string strdate,int isession,bool bAvaility);
//	int     Delete(string strdate);
//	int     Delete(string strdate,int isession);
//	int     Clear();
//};

class CBindCard
{
	public:
	BOOL Insert(bindcard_st &card); //制卡
	void Query(int id,string &strname); //查订单号
	void Query_bindinfo(int id,string &strname,string &strmaketime);//查询绑定时间及订单号
	void Query(vector<bindcard_st> &out);
	BOOL Update(int id,bindcard_st &out);
	int  Reset(int id);
	int  Delete(int id);
	int  Clear();
};

//账户信息
class CLogBox{
public:
	BOOL  Insert(loguser_st &account);
	void  Query(string username,loguser_st &account); //查询某个用户的信息
	void  Query(vector<loguser_st> &out);//查询所有用户信息
	BOOL  Update(string username,loguser_st &account);//修改某个用户的信息
	int   Delete(string username);
	int   Clear();//删除所有用户
};

//成绩
class CScoreBox
{
public:
	BOOL  Insert(userscore_st &score);
	void  Query_by_phone(string phonenumber,userscore_st &score); //通过手机号
	void  Query_by_Ordernumber(string number,userscore_st &score);//通过订单号
	int   Clear();
};

//成绩关联邮箱发送
class CScoreMailBox
{
   public:
	  BOOL Insert(userscore_st &score);
	  int  Clear();
};

//制卡时间_入馆体验顺序
class CPlayNumber
{
   public:
	   BOOL Insert(playnumber_st &info);
	   void Query_id(string strordernumber,int &id); //得到制卡序号，即第几个入馆体验的
	   int  Clear();
};


class CBDStorage:
	public CAccountBox,
	public COrderFormBox,
	public CTicketResource,
	public CBindCard,
	public CLogBox,
	public CScoreMailBox,
	public CScoreBox,
	public CPlayNumber

{
public:
	CBDStorage(void);
	~CBDStorage(void);				
protected:
	
private:
	CIniWrap m_initWrap;
	string m_strUrl;
	
public:
	string m_strServiceIP; //状态服务器IP
	unsigned int m_ServicePort;//状态服务器端口
	unsigned int m_opentimeout;
	unsigned int m_readtimeout;
	string GetUrl();
	unsigned int GetReadTimeOut();
	unsigned int GetOpenTimeOut();

};

