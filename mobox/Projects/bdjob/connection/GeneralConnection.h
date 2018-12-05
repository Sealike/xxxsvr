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
	
	//设置连接描述字符串
	//strDescrip,用户描述字符串；
	//返回值：无；
	virtual void SetDescript(const TCHAR* strDescrip){ m_strDescription = strDescrip;}
	
	//获取连接描述字符串
	//返回值：用户描述字符串；调用方获得该字符串后，不要修改；
	virtual const TCHAR* GetDescript()const { return m_strDescription.c_str(); }
	
	//设置转发连接
	//pConnection,转发目标连接；
	//返回值，目前的转发连接；
	virtual IConnection* TransitConnect(IConnection * pConnection);
	
protected:
	//转发发送
	//chBuffer,待转发的缓冲区，
	//nDataLen,数据缓冲区的长度。
	//返回值，目前的转发连接；
	virtual IConnection* TransitSend(const unsigned char chBuffer[] , const int nDataLen);
    
private:
	//描述字符串成员变量；
	basic_string<TCHAR>  m_strDescription;
	//转发连接属性；
	IConnection *  m_pTransConnection;
};
