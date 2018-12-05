#pragma once
#include "messagess.h"
#include <vector>
#include "DbQuery.h"
#include "IniWrap.h"

using namespace std;


//�˻���Ϣ
class CAccountBox{
public:
	BOOL  Insert(account_st &account);
	void  Query(string username,account_st &account); //��ѯĳ���û�����Ϣ
	void  Query(vector<account_st> &out);//��ѯ�����û���Ϣ
	BOOL  Update(string username,account_st &account);//�޸�ĳ���û�����Ϣ
	int   Delete(string username);
	int   Clear();//ɾ�������û�
};

//�������
class COrderFormBox
{
	public:
	BOOL Insert(orderform_st &orderform);
	void Query(string ordernumber,string ordercode,orderform_st &orderform);//��ѯĳ��������Ϣ
	void Query(string ordernumber,orderform_st &orderform);//
	void Query(vector<orderform_st> &out);//��ѯ���ж���
	void Query_by_phone(string phone,vector<orderform_st> &out);//�����ֻ��Ų�ѯ
	BOOL Update(string ordernumber,orderform_st &orderform);//�޸�ĳ��������Ϣ
	BOOL Update(string ordernumber,int payment);//֧��
	BOOL Update_use(string ordernumber,int ibuse);//�Ƿ���ʹ��
	int  Delete(string ordernumber);
	int  Clear();
};

//Ʊ��Դ
class CTicketResource
{
	public:
	BOOL  Insert(ticketresource_st &ticketinfo);
	void  Query(string strdate,vector<ticketresource_st> &ticketinfo);
	void  Query_span(string startdate,string enddate,vector<ticketresource_st> &ticketinfo);
	void  Query(string strdate,int isession,int &iSum,int &iRemain);//��ѯƱ��
	void  Query(string strdate,int isession,string &strStartTime,string &strEndTime);
	void  Query(vector<ticketresource_st> &out);//
	void  Query_Session(string startdate,string strStartTime,string strEndTime,int &isession);//��ѯĳ��ʱ���Ƿ��Ѿ�������Ϊ�ض�����
	BOOL  Update(string strdate,int isession,ticketresource_st &ticketinfo);//
	BOOL  Update(string strdate,int isession,int iuse);//
	int   Decrease(string strdate,int isession,int icount);//����n��Ʊ
	int   Increase(string strdate,int isession,int icount);//����n��Ʊ
	int   Delete(string strdate,int isession);
	int   Delete(string strdate);
	int   Delete(string startdate,string enddate);
	int   Clear();


};


//����
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
	BOOL Insert(bindcard_st &card); //�ƿ�
	void Query(int id,string &strname); //�鶩����
	void Query_bindinfo(int id,string &strname,string &strmaketime);//��ѯ��ʱ�估������
	void Query(vector<bindcard_st> &out);
	BOOL Update(int id,bindcard_st &out);
	int  Reset(int id);
	int  Delete(int id);
	int  Clear();
};

//�˻���Ϣ
class CLogBox{
public:
	BOOL  Insert(loguser_st &account);
	void  Query(string username,loguser_st &account); //��ѯĳ���û�����Ϣ
	void  Query(vector<loguser_st> &out);//��ѯ�����û���Ϣ
	BOOL  Update(string username,loguser_st &account);//�޸�ĳ���û�����Ϣ
	int   Delete(string username);
	int   Clear();//ɾ�������û�
};

//�ɼ�
class CScoreBox
{
public:
	BOOL  Insert(userscore_st &score);
	void  Query_by_phone(string phonenumber,userscore_st &score); //ͨ���ֻ���
	void  Query_by_Ordernumber(string number,userscore_st &score);//ͨ��������
	int   Clear();
};

//�ɼ��������䷢��
class CScoreMailBox
{
   public:
	  BOOL Insert(userscore_st &score);
	  int  Clear();
};

//�ƿ�ʱ��_�������˳��
class CPlayNumber
{
   public:
	   BOOL Insert(playnumber_st &info);
	   void Query_id(string strordernumber,int &id); //�õ��ƿ���ţ����ڼ�����������
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
	string m_strServiceIP; //״̬������IP
	unsigned int m_ServicePort;//״̬�������˿�
	unsigned int m_opentimeout;
	unsigned int m_readtimeout;
	string GetUrl();
	unsigned int GetReadTimeOut();
	unsigned int GetOpenTimeOut();

};

