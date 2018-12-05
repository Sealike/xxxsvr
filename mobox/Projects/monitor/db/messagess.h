#ifndef __MESSAHE_H__
#define __MESSAHE_H__
#include <string>
#include "type.h"
typedef int BOOL;

using namespace std;

//�˻����
struct account_st 
{
	string username; //�ֻ���
	string pwd;
	string strname; //�ǳ�
	int    iage;//����
	string strworkunit;// ������λ
	string strmail;//����
};

//�������
struct orderform_st{
	string strdate;  //���� YYYY-MM-DD
	int isession;   //����
	string strusername;  //�û���:�ֻ���
	string strname; //�ǳ�
	int    iage;//����
	string strworkunit;// ������λ
	string strmail;//����
	int iordercode;//��Ʊ����
	string strordernumber; //������
	string strIdentify;//��֤��
	int  iordercount;//���˼���Ʊ
	int iordertype; //��������
	int ibuse;   //�Ƿ���ʹ��
	string strordertime;//��Ʊʱ��
	int payment;//�Ƿ���֧�� 0δ֧��  1��֧��
};

//�û�����ɼ�
struct userscore_st
{
	string strdate;  //���� YYYY-MM-DD
	int isession;   //����
	string strusername;  //�û���:�ֻ���
	string strname; //�ǳ�
	int    iage;//����
	string strworkunit;// ������λ
	string strmail;//����
	string strordernumber; //������
	string score;
	string travescence; //������Щ����
	
};
//Ʊ��Դ
struct ticketresource_st{
	string strdate;  //���� YYYY-MM-DD
	int isession;

	int iticketsum;//
	int iticketremain;
	string strstarttime; //��ʼʱ��
	string strendtime; //����ʱ��
	int    iavaility;
};

//����<����ʹ��>
struct sessioninfo_st 
{
	string strdate;
	int    isession;
	string strstarttime;
	string strendtime;
	int    iavaility;
};

//��
struct bindcard_st 
{	
	int icardid;
	string strusername; //������
	string strmaketime;//�ƿ�ʱ��
};

struct loguser_st
{
	string username;
	string pwd;
};

//�ο�����˳��,�ƿ�ʱ��
struct playnumber_st
{
	int id;
	string strordernumber;
	string strmaketime;//�ƿ�ʱ��
};

#endif