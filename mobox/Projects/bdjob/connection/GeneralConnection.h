#pragma once

#include <string>
//#include "../BDProtocal/protocal/ChnavtectGlobal.h"
#include "ChnavtectGlobal.h"
using namespace std;

class CGeneralConnection :public IConnection
{
protected:
	CGeneralConnection(void);
	~CGeneralConnection(void);
public:
	
	//�������������ַ���
	//strDescrip,�û������ַ�����
	//����ֵ���ޣ�
	virtual void SetDescript(const TCHAR* strDescrip){ m_strDescription = strDescrip;}
	
	//��ȡ���������ַ���
	//����ֵ���û������ַ��������÷���ø��ַ����󣬲�Ҫ�޸ģ�
	virtual const TCHAR* GetDescript()const { return m_strDescription.c_str(); }
	
	//����ת������
	//pConnection,ת��Ŀ�����ӣ�
	//����ֵ��Ŀǰ��ת�����ӣ�
	virtual IConnection* TransitConnect(IConnection * pConnection);
	
protected:
	//ת������
	//chBuffer,��ת���Ļ�������
	//nDataLen,���ݻ������ĳ��ȡ�
	//����ֵ��Ŀǰ��ת�����ӣ�
	virtual IConnection* TransitSend(const unsigned char chBuffer[] , const int nDataLen);
    
private:
	//�����ַ�����Ա������
	basic_string<TCHAR>  m_strDescription;
	//ת���������ԣ�
	IConnection *  m_pTransConnection;
};
