#include "stdafx.h"
#include "BDStorage.h"
#include "DbQuery.h"
#include "Function.h"
#include <time.h>
#include <algorithm>
using namespace std;

static CDbQuery g_query_local;

class CDBStorage;

BOOL  CAccountBox::Insert(account_st &account)
{
	BOOL rtn = FALSE;

	char strvalue[512];
	sprintf_s(strvalue,sizeof(strvalue),"'%s','%s','%s',%d,'%s','%s'",account.username.c_str(),account.pwd.c_str(),account.strname.c_str(),account.iage,account.strworkunit.c_str(),account.strmail.c_str());		
	rtn = g_query_local.GetCQueryAccountImpl()->cond_insert("username,pwd,name,age,workunit,mail",strvalue);
	return rtn;
}

void  CAccountBox::Query(string username,account_st &account)
{
	vector<account_st> out;
	char strcond[512];
	memset(&account,0,sizeof(account_st));

	sprintf_s(strcond,sizeof(strcond),"username='%s'",username.c_str());
	g_query_local.GetCQueryAccountImpl()->cond_query(strcond,&out);

	if(out.size()>0)
		account = out[0];
}

void  CAccountBox::Query(vector<account_st> &out)
{
	g_query_local.GetCQueryAccountImpl()->cond_query(NULL,&out);
}

BOOL  CAccountBox::Update(string username,account_st &account)
{
	BOOL rtn = FALSE;
	char setting[256];
	char cond[256];

	sprintf_s(setting,sizeof(setting),"username='%s',pwd='%s',name='%s',age=%d,workunit='%s',mail='%s'",account.username.c_str(),account.pwd.c_str(),account.strname.c_str(),account.iage,account.strworkunit.c_str(),account.strmail.c_str());	
	sprintf_s(cond,sizeof(cond),"username='%s'",username.c_str());	

	rtn = g_query_local.GetCQueryAccountImpl()->cond_update(setting,cond);

	return rtn;
}

int   CAccountBox::Delete(string username)
{
	char strcond[512];
	sprintf_s(strcond,sizeof(strcond),"username='%s'",username.c_str());
	return g_query_local.GetCQueryAccountImpl()->cond_delete(strcond);
}

int   CAccountBox::Clear()
{
	return g_query_local.GetCQueryAccountImpl()->cond_delete(NULL);
}


BOOL COrderFormBox::Insert(orderform_st &orderform)
{
	BOOL rtn = FALSE;
	char strvalue[512];
	sprintf_s(strvalue,sizeof(strvalue),"'%s',%d,'%s','%s',%d,'%s','%s',%d ,'%s','%s',%d,%d,'%s','%s',%d",orderform.strdate.c_str(),orderform.isession,orderform.strusername.c_str(),orderform.strname.c_str(),orderform.iage,orderform.strworkunit.c_str(),orderform.strmail.c_str(),orderform.iordercount,orderform.strordernumber.c_str(),orderform.strIdentify.c_str(),orderform.iordertype,orderform.ibuse,orderform.strordertime.c_str(),"",orderform.payment);		
	rtn = g_query_local.GetCQueryOrderFormImpl()->cond_insert("orderdate,session,username,nickame,age,workunit,mail,ordersum,ordernumber,identifycode,ordertype,buse,time,useridentify,payment",strvalue);
	return rtn;
}

void COrderFormBox::Query(string ordernumber,string strordercode,orderform_st &orderform)
{
	vector<orderform_st> out;
	char strcond[512];
	memset(&orderform,0,sizeof(orderform_st));

	sprintf_s(strcond,sizeof(strcond),"ordernumber='%s'AND identifycode='%s'",ordernumber.c_str(),strordercode.c_str());
	g_query_local.GetCQueryOrderFormImpl()->cond_query(strcond,&out);
	if(out.size()>0)
		orderform = out[0];
}
void COrderFormBox::Query(string ordernumber,orderform_st &orderform)
{
	vector<orderform_st> out;
	char strcond[512];
	memset(&orderform,0,sizeof(orderform_st));

	sprintf_s(strcond,sizeof(strcond),"ordernumber='%s'",ordernumber.c_str());
	g_query_local.GetCQueryOrderFormImpl()->cond_query(strcond,&out);
	if(out.size()>0)
		orderform = out[0];
}
void  COrderFormBox::Query(vector<orderform_st> &out)
{
	g_query_local.GetCQueryOrderFormImpl()->cond_query(NULL,&out);
}
void COrderFormBox::Query_by_phone(string phone,vector<orderform_st> &out)//根据手机号查询
{
	char strcond[512];
	sprintf_s(strcond,sizeof(strcond),"username='%s' order by time desc",phone.c_str());
	g_query_local.GetCQueryOrderFormImpl()->cond_query(strcond,&out);

}
BOOL COrderFormBox::Update(string ordernumber,orderform_st &orderform)
{
	BOOL rtn = FALSE;
	char setting[256];
	char cond[256];

	sprintf_s(setting,sizeof(setting),"orderdate='%s',session=%d,username='%s',ordernumber='%s',ordertype=%d,buse=%d",orderform.strdate.c_str(),orderform.isession,orderform.strusername.c_str(),orderform.strordernumber.c_str(),orderform.iordertype,orderform.ibuse);	
	sprintf_s(cond,sizeof(cond),"ordernumber='%s'",ordernumber.c_str());	

	rtn = g_query_local.GetCQueryOrderFormImpl()->cond_update(setting,cond);

	return rtn;
}


BOOL COrderFormBox::Update(string ordernumber,int payment)//支付
{
	BOOL rtn = FALSE;
	char setting[256];
	char cond[256];

	sprintf_s(setting,sizeof(setting),"payment=%d",payment);	
	sprintf_s(cond,sizeof(cond),"ordernumber='%s'",ordernumber.c_str());	

	rtn = g_query_local.GetCQueryOrderFormImpl()->cond_update(setting,cond);

	return rtn;
}

BOOL COrderFormBox::Update_use(string ordernumber,int ibuse)//是否已使用
{
	BOOL rtn = FALSE;
	char setting[256];
	char cond[256];

	sprintf_s(setting,sizeof(setting),"buse=%d",ibuse);	
	sprintf_s(cond,sizeof(cond),"ordernumber='%s'",ordernumber.c_str());	

	rtn = g_query_local.GetCQueryOrderFormImpl()->cond_update(setting,cond);

	return rtn;
}

int  COrderFormBox::Delete(string ordernumber)
{
	char strcond[512];
	sprintf_s(strcond,sizeof(strcond),"ordernumber='%s'",ordernumber.c_str());
	return g_query_local.GetCQueryOrderFormImpl()->cond_delete(strcond);
}

int  COrderFormBox::Clear()
{
	return g_query_local.GetCQueryOrderFormImpl()->cond_delete(NULL);
}

BOOL  CTicketResource::Insert(ticketresource_st &ticketinfo)
{
	BOOL rtn = FALSE;
	char strvalue[512];
	sprintf_s(strvalue,sizeof(strvalue),"'%s',%d,%d,%d,'%s','%s',%d",ticketinfo.strdate.c_str(),ticketinfo.isession,ticketinfo.iticketsum,ticketinfo.iticketremain,ticketinfo.strstarttime.c_str(),ticketinfo.strendtime.c_str(),ticketinfo.iavaility);		
	rtn = g_query_local.GetCQueryTicketImpl()->cond_insert("ticketdate,session,ticketsum ,ticketremain,timestart,timeend ,bavaility",strvalue);
	return rtn;
}
void  CTicketResource::Query(string strdate,vector<ticketresource_st> &ticketinfo)
{
	vector<ticketresource_st> out;
	char strcond[512];
	sprintf_s(strcond,sizeof(strcond),"ticketdate='%s' order by session asc ",strdate.c_str());
	g_query_local.GetCQueryTicketImpl()->cond_query(strcond,&out);
	if(out.size()>0)
		ticketinfo = out;
}

void CTicketResource::Query_span(string startdate,string enddate,vector<ticketresource_st> &ticketinfo)
{
	vector<ticketresource_st> out;
	char strcond[512];
	sprintf_s(strcond,sizeof(strcond),"ticketdate>='%s' AND ticketdate<='%s' order by ticketdate asc,session",startdate.c_str(),enddate.c_str());
	g_query_local.GetCQueryTicketImpl()->cond_query(strcond,&out);
	if(out.size()>0)
		ticketinfo = out;

}

void  CTicketResource::Query_Session(string startdate,string strStartTime,string strEndTime,int &isession)//查询某个时间是否已经被设置为特定场次
{
	vector<ticketresource_st> out;
	char strcond[512];
	sprintf_s(strcond,sizeof(strcond),"ticketdate='%s' AND timestart>='%s' AND timeend <= '%s'",startdate.c_str(),strStartTime.c_str(),strEndTime.c_str());
	g_query_local.GetCQueryTicketImpl()->cond_query(strcond,&out);
	if(out.size()>0)
		isession = out[0].isession;
}
void  CTicketResource::Query(string strdate,int isession,int &iSum,int &iRemain)
{
	vector<ticketresource_st> out;
	char strcond[512];
	iSum = 0;
	iRemain = 0;
	sprintf_s(strcond,sizeof(strcond),"ticketdate='%s'AND session=%d",strdate.c_str(),isession);
	g_query_local.GetCQueryTicketImpl()->cond_query(strcond,&out);
	if(out.size()>0)
	{
		iSum = out[0].iticketsum;
		iRemain = out[0].iticketremain;
	}
}

void  CTicketResource::Query(string strdate,int isession,string &strStartTime,string &strEndTime)
{
	vector<ticketresource_st> out;
	char strcond[512];
	sprintf_s(strcond,sizeof(strcond),"ticketdate='%s' AND session=%d",strdate.c_str(),isession);
	g_query_local.GetCQueryTicketImpl()->cond_query(strcond,&out);
	if(out.size()>0)
	{
		strStartTime = out[0].strstarttime;
		strEndTime   = out[0].strendtime;
	}
}
void  CTicketResource::Query(vector<ticketresource_st> &out)
{
	g_query_local.GetCQueryTicketImpl()->cond_query(NULL,&out);
}

BOOL  CTicketResource::Update(string strdate,int isession,int iuse)
{
	BOOL rtn = FALSE;
	char setting[256];
	char cond[256];

	sprintf_s(setting,sizeof(setting),"ticketdate='%s',session=%d,bavaility=%d",strdate.c_str(),isession,iuse);	
	sprintf_s(cond,sizeof(cond),"ticketdate='%s' AND session=%d",strdate.c_str(),isession);	
	rtn = g_query_local.GetCQueryTicketImpl()->cond_update(setting,cond);
	return rtn;
}
BOOL  CTicketResource::Update(string strdate,int isession,ticketresource_st &ticketinfo)
{
	BOOL rtn = FALSE;
	char setting[256];
	char cond[256];

	sprintf_s(setting,sizeof(setting),"ticketdate='%s',session=%d,ticketsum=%d ,ticketremain=%d",ticketinfo.strdate.c_str(),ticketinfo.isession,ticketinfo.iticketsum,ticketinfo.iticketremain);	
	sprintf_s(cond,sizeof(cond),"ticketdate='%s' AND session=%d",strdate.c_str(),isession);	
	rtn = g_query_local.GetCQueryTicketImpl()->cond_update(setting,cond);
	return rtn;
}
int   CTicketResource::Decrease(string strdate,int isession,int icount)
{
	BOOL rtn = FALSE;
	char setting[256];
	char cond[256];

	vector<ticketresource_st> out;
	char strcond[512];
	int iRemain = 0;
	int iSum = 0;
	sprintf_s(strcond,sizeof(strcond),"ticketdate='%s' AND session=%d",strdate.c_str(),isession);
	g_query_local.GetCQueryTicketImpl()->cond_query(strcond,&out);
	if(out.size()>0)
	{
		iSum    =  out[0].iticketsum;
		iRemain = out[0].iticketremain;
	}

	if(iRemain - icount >= 0)
		iRemain = iRemain - icount;
	else
		return -1;//失败
	sprintf_s(setting,sizeof(setting),"ticketdate='%s',session=%d,ticketremain=%d",strdate.c_str(),isession,iRemain);	
	sprintf_s(cond,sizeof(cond),"ticketdate='%s'AND session=%d",strdate.c_str(),isession);	
	rtn = g_query_local.GetCQueryTicketImpl()->cond_update(setting,cond);
	return rtn;
}
int   CTicketResource::Increase(string strdate,int isession,int icount)
{
	BOOL rtn = FALSE;
	char setting[256];
	char cond[256];

	vector<ticketresource_st> out;
	char strcond[512];
	int iRemain = 0;
	int iSum = 0;
	sprintf_s(strcond,sizeof(strcond),"ticketdate='%s' AND session=%d",strdate.c_str(),isession);
	g_query_local.GetCQueryTicketImpl()->cond_query(strcond,&out);
	if(out.size()>0)
	{
		iSum    =  out[0].iticketsum;
		iRemain = out[0].iticketremain;
	}

	if(iRemain + icount)
		iRemain = iRemain + icount;
	sprintf_s(setting,sizeof(setting),"ticketdate='%s',session=%d,ticketremain=%d",strdate.c_str(),isession,iRemain);	
	sprintf_s(cond,sizeof(cond),"ticketdate='%s'AND session=%d",strdate.c_str(),isession);	
	rtn = g_query_local.GetCQueryTicketImpl()->cond_update(setting,cond);
	return rtn;
}
int   CTicketResource::Delete(string strdate,int isession)
{
	char strcond[512];
	sprintf_s(strcond,sizeof(strcond),"ticketdate='%s' AND session=%d",strdate.c_str(),isession);
	return g_query_local.GetCQueryTicketImpl()->cond_delete(strcond);
}

int   CTicketResource::Delete(string strdate)
{
	char strcond[512];
	sprintf_s(strcond,sizeof(strcond),"ticketdate='%s'",strdate.c_str());
	return g_query_local.GetCQueryTicketImpl()->cond_delete(strcond);
}

int   CTicketResource::Delete(string startdate,string enddate)
{
	char strcond[512];
	sprintf_s(strcond,sizeof(strcond),"ticketdate>='%s' AND ticketdate<='%s'",startdate.c_str(),enddate.c_str());
	return g_query_local.GetCQueryTicketImpl()->cond_delete(strcond);
}

int   CTicketResource::Clear()
{
	return g_query_local.GetCQueryTicketImpl()->cond_delete(NULL);
}

//废弃使用
//BOOL   CSessionInfo::Insert(sessioninfo_st &sessioninfo)
//{
//	BOOL rtn = FALSE;
//	char strvalue[512];
//	sprintf_s(strvalue,sizeof(strvalue),"'%s',%d,'%s','%s',%d",sessioninfo.strdate.c_str(),sessioninfo.isession,sessioninfo.strstarttime.c_str(),sessioninfo.strendtime.c_str(),sessioninfo.iavaility);		
//	rtn = g_query_local.GetCQuerySessionImpl()->cond_insert("ticketdate,session,timestart,timeend,bavaility",strvalue);
//	return rtn;
//}
//
//void   CSessionInfo::Query(string strdate,vector<sessioninfo_st> &ticketinfo)
//{
//	vector<sessioninfo_st> out;
//	char strcond[512];
//	memset(&out,0,sizeof(sessioninfo_st));
//
//	sprintf_s(strcond,sizeof(strcond),"ticketdate='%s'",strdate.c_str());
//	g_query_local.GetCQuerySessionImpl()->cond_query(strcond,&out);
//	if(out.size()>0)
//		ticketinfo = out;
//}
//
//void   CSessionInfo::Query(string strdate,int isession,sessioninfo_st &ticketinfo)
//{
//	char strcond[512];
//	sprintf_s(strcond,sizeof(strcond),"ticketdate='%s',session=%d",strdate.c_str(),isession);
//	g_query_local.GetCQuerySessionImpl()->cond_query(strcond,&ticketinfo);
//}
//
//BOOL   CSessionInfo::Update(string strdate,int isession,sessioninfo_st &out)
//{
//	BOOL rtn = FALSE;
//	char setting[256];
//	char cond[256];
//
//	sprintf_s(setting,sizeof(setting),"ticketdate = '%s',session=%d,timestart ='%s',timeend='%s',bavaility=%d",out.strdate.c_str(),out.isession,out.strstarttime.c_str(),out.strendtime.c_str(),out.iavaility);	
//	sprintf_s(cond,sizeof(cond),"ticketdate='%s',session=%d",strdate.c_str(),isession);	
//	rtn = g_query_local.GetCQuerySessionImpl()->cond_update(setting,cond);
//	return rtn;
//}
//
//void   CSessionInfo::SetEnable(string strdate,int isession,bool bAvaility)
//{
//	char setting[256];
//	char cond[256];
//
//	sprintf_s(setting,sizeof(setting),"bavaility=%d",bAvaility);	
//	sprintf_s(cond,sizeof(cond),"ticketdate='%s',session=%d",strdate.c_str(),isession);	
//	g_query_local.GetCQuerySessionImpl()->cond_update(setting,cond);
//}
//
//int   CSessionInfo::Delete(string strdate)
//{
//	char strcond[512];
//	sprintf_s(strcond,sizeof(strcond),"ticketdate='%s'",strdate.c_str());
//	return g_query_local.GetCQuerySessionImpl()->cond_delete(strcond);
//}
//
//int   CSessionInfo::Delete(string strdate,int isession)
//{
//	char strcond[512];
//	sprintf_s(strcond,sizeof(strcond),"ticketdate='%s',session=%d",strdate.c_str(),isession);
//	return g_query_local.GetCQuerySessionImpl()->cond_delete(strcond);
//}
//
//int   CSessionInfo::Clear()
//{
//	return g_query_local.GetCQuerySessionImpl()->cond_delete(NULL);
//}

BOOL CBindCard::Insert(bindcard_st &card)
{
	BOOL rtn = FALSE;
	char strvalue[512];
	sprintf_s(strvalue,sizeof(strvalue),"%d,'%s','%s'",card.icardid,card.strusername.c_str(),card.strmaketime.c_str());		
	rtn = g_query_local.GetCQueryCardImpl()->cond_insert("cardid,ordernumber,maketime",strvalue);
	return rtn;
}
void CBindCard::Query(int id,string &name)
{
	char strcond[512];
	vector <bindcard_st> out;
	sprintf_s(strcond,sizeof(strcond),"cardid=%d",id);
	g_query_local.GetCQueryCardImpl()->cond_query(strcond,&out);
	if(out.size() >0)
	name = out[0].strusername;
}

void CBindCard::Query_bindinfo(int id,string &strname,string &strmaketime)//查询绑定时间
{
	char strcond[512];
	vector <bindcard_st> out;
	sprintf_s(strcond,sizeof(strcond),"cardid=%d",id);
	g_query_local.GetCQueryCardImpl()->cond_query(strcond,&out);
	if(out.size() >0)
	{
		strname = out[0].strusername;
		strmaketime = out[0].strmaketime;
	}
}
void CBindCard::Query(vector<bindcard_st> &out)
{
	g_query_local.GetCQueryCardImpl()->cond_query(NULL,&out);
}
BOOL CBindCard::Update(int id,bindcard_st &out)
{
	BOOL rtn = FALSE;
	char setting[256];
	char cond[256];

	sprintf_s(setting,sizeof(setting),"cardid=%d,ordernumber ='%s',maketime='%s'",out.icardid,out.strusername.c_str(),out.strmaketime.c_str());	
	sprintf_s(cond,sizeof(cond),"cardid=%d",id);	
	rtn = g_query_local.GetCQueryCardImpl()->cond_update(setting,cond);
	return rtn;
}

int  CBindCard::Reset(int id)
{
	BOOL rtn = FALSE;
	char setting[256];
	char cond[256];

	sprintf_s(setting,sizeof(setting),"cardid=%d,ordernumber ='%s',maketime='%s'",id,"","");	
	sprintf_s(cond,sizeof(cond),"cardid=%d",id);	
	rtn = g_query_local.GetCQueryCardImpl()->cond_update(setting,cond);
	return rtn;
}
int  CBindCard::Delete(int id)
{
	char strcond[512];
	sprintf_s(strcond,sizeof(strcond),"cardid =%d",id);
	return g_query_local.GetCQueryCardImpl()->cond_delete(strcond);
}
int  CBindCard::Clear()
{
	return g_query_local.GetCQueryCardImpl()->cond_delete(NULL);
}


BOOL  CLogBox::Insert(loguser_st &account)
{
	BOOL rtn = FALSE;

	char strvalue[512];
	sprintf_s(strvalue,sizeof(strvalue),"'%s','%s'",account.username.c_str(),account.pwd.c_str());		
	rtn = g_query_local.GetCQueryLogUserImpl()->cond_insert("username,psw",strvalue);

	return rtn;
}

void  CLogBox::Query(string username,loguser_st &account)
{
	vector<loguser_st> out;
	char strcond[512];
	memset(&account,0,sizeof(loguser_st));

	sprintf_s(strcond,sizeof(strcond),"username='%s'",username.c_str());
	g_query_local.GetCQueryLogUserImpl()->cond_query(strcond,&out);

	if(out.size()>0)
		account = out[0];
}

void  CLogBox::Query(vector<loguser_st> &out)
{
	g_query_local.GetCQueryLogUserImpl()->cond_query(NULL,&out);
}

BOOL  CLogBox::Update(string username,loguser_st &account)
{
	BOOL rtn = FALSE;
	char setting[256];
	char cond[256];

	sprintf_s(setting,sizeof(setting),"username='%s',psw='%s'",account.username.c_str(),account.pwd.c_str());	
	sprintf_s(cond,sizeof(cond),"username='%s'",username.c_str());	

	rtn = g_query_local.GetCQueryLogUserImpl()->cond_update(setting,cond);

	return rtn;
}

int   CLogBox::Delete(string username)
{
	char strcond[512];
	sprintf_s(strcond,sizeof(strcond),"username='%s'",username.c_str());
	return g_query_local.GetCQueryLogUserImpl()->cond_delete(strcond);
}

int   CLogBox::Clear()
{
	return g_query_local.GetCQueryLogUserImpl()->cond_delete(NULL);
}

BOOL CScoreBox::Insert(userscore_st &scoreinfo)
{
	BOOL rtn = FALSE;

	char *strvalue = new char[1024*64];
	sprintf_s(strvalue,1024*64,"'%s',%d,'%s','%s',%d,'%s','%s','%s','%s','%s'",scoreinfo.strdate.c_str(),scoreinfo.isession,scoreinfo.strusername.c_str(),scoreinfo.strname.c_str(),scoreinfo.iage,scoreinfo.strworkunit.c_str(),scoreinfo.strmail.c_str(),scoreinfo.strordernumber.c_str(),scoreinfo.score.c_str(),scoreinfo.travescence.c_str());		
	rtn = g_query_local.GetCQueryScoreImpl()->cond_insert("orderdate,session,username,nickame,age,workunit,mail,ordernumber,score,travelscence",strvalue);
	delete strvalue;
	return rtn;
}

void  CScoreBox::Query_by_phone(string phonenumber,userscore_st &score) //通过手机号
{
	vector<userscore_st> out;
	char strcond[512];
	memset(&out,0,sizeof(userscore_st));

	sprintf_s(strcond,sizeof(strcond),"username='%s'",phonenumber.c_str());
	g_query_local.GetCQueryScoreImpl()->cond_query(strcond,&out);

	if(out.size()>0)
		score = out[0];
}

void  CScoreBox::Query_by_Ordernumber(string number,userscore_st &score)//通过订单号
{
	vector<userscore_st> out;
	char strcond[512];
	memset(&out,0,sizeof(userscore_st));

	sprintf_s(strcond,sizeof(strcond),"ordernumber='%s'",number.c_str());
	g_query_local.GetCQueryScoreImpl()->cond_query(strcond,&out);

	if(out.size()>0)
		score = out[0];
}

int   CScoreBox::Clear()
{
	return g_query_local.GetCQueryScoreImpl()->cond_delete(NULL);
}


BOOL CScoreMailBox::Insert(userscore_st &scoreinfo)
{
	BOOL rtn = FALSE;
	char *strvalue = new char[1024*64];
	sprintf_s(strvalue,1024*64,"'%s',%d,'%s','%s',%d,'%s','%s','%s','%s','%s'",scoreinfo.strdate.c_str(),scoreinfo.isession,scoreinfo.strusername.c_str(),scoreinfo.strname.c_str(),scoreinfo.iage,scoreinfo.strworkunit.c_str(),scoreinfo.strmail.c_str(),scoreinfo.strordernumber.c_str(),scoreinfo.score.c_str(),scoreinfo.travescence.c_str());		
	rtn = g_query_local.GetCQueryMailScoreImpl()->cond_insert("orderdate,session,username,nickame,age,workunit,mail,ordernumber,score,travelscence",strvalue);
	delete strvalue;
	return rtn;
}

int   CScoreMailBox::Clear()
{
	return g_query_local.GetCQueryMailScoreImpl()->cond_delete(NULL);
}


BOOL CPlayNumber::Insert(playnumber_st &info)
{
	BOOL rtn = FALSE;
	char strvalue[512];
	sprintf_s(strvalue,sizeof(strvalue),"'%s','%s'",info.strordernumber.c_str(),info.strmaketime.c_str());		
	rtn = g_query_local.GetCQueryPlayNumberImpl()->cond_insert("ordernumber,maketime",strvalue);
	return rtn;
}

void CPlayNumber::Query_id(string strordernumber,int &id)
{
	char strcond[512];
	vector <playnumber_st> out;
	sprintf_s(strcond,sizeof(strcond),"ordernumber='%s'",strordernumber.c_str());
	g_query_local.GetCQueryPlayNumberImpl()->cond_query(strcond,&out);
	if(out.size() >0)
		id = out[0].id;
}
int  CPlayNumber::Clear()
{
	return g_query_local.GetCQueryPlayNumberImpl()->cond_delete(NULL);
}
CBDStorage::CBDStorage(void)
{	
	char dbname[128];
	char svrname[64];
	int  svrport;
	int  readtimeouts; //单位为毫秒，http doget函数的参数
	int opentimeouts;
	char username[64];
	char userpwd[64];
	char strurl[64];
	char strServiceIP[64];
    int  iStatePort = 0;
	m_initWrap.SetFile("../etc/config.ini");	
	m_initWrap.ReadStr("server","serverip",svrname,sizeof(svrname));
	m_initWrap.ReadInt("server","serverport",svrport,-1);
	m_initWrap.ReadStr("server","user",username,sizeof(username));
	m_initWrap.ReadStr("server","pwd",userpwd,sizeof(userpwd));
	m_initWrap.ReadInt("server","opentimeout",opentimeouts,-1);
	m_initWrap.ReadInt("server","readtimeout",readtimeouts,-1);
	m_initWrap.ReadStr("server","score_url",strurl,sizeof(strurl));
	
	//m_initWrap.ReadStr("server","stateserviceip",strServiceIP,sizeof(strServiceIP));
	//m_initWrap.ReadInt("server","stateserviceport",iStatePort,-1);
	m_strUrl = strurl;
	m_strServiceIP = strServiceIP;
	m_ServicePort = iStatePort;
	m_opentimeout = opentimeouts;
	m_readtimeout = readtimeouts;
	int rslt = g_query_local.db_connect(svrname,svrport,username,userpwd,"emergency_info_db",TRUE,120);
	if(!rslt)
		printf("连接失败!\n");
}

string CBDStorage::GetUrl()
{
	return m_strUrl;
}

unsigned int CBDStorage::GetReadTimeOut()
{
	return m_readtimeout;
}
unsigned int CBDStorage::GetOpenTimeOut()
{
	return m_opentimeout;
}
CBDStorage::~CBDStorage(void)
{
	g_query_local.db_close();
}
