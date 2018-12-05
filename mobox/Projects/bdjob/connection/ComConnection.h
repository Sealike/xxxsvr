#pragma once
#include "GeneralConnection.h"

/*
不具备多线程特性；
没有应用经历；
没有测试；
*/

#include <string>
using namespace std;

class CComConnection : public CGeneralConnection
{   
	CComConnection(void);
	virtual ~CComConnection(void);
public:	
	
/*  paritytype..
	EVENPARITY 
	MARKPARITY   
	NOPARITY 
	ODDPARITY
	SPACEPARITY 
*/

/*  stopbits..
	ONESTOPBIT 1 stop bit 
	ONE5STOPBITS 1.5 stop bits 
	TWOSTOPBITS 2 stop bits 
*/

	//创建连接函数...
	//nPort，目标端口号
	//nBaudRate,打开串口的波特率；
	//nDataBits,数据位的个数；
	//nStopBits,停止位的个数；
	//nParity， 校验位的方式；
	//ppConnetion：输出参数，创建的链接指针；
	//返回值：创建成功，返回TRUE,否则：返回FALSE；
	static BOOL  OpenAConnection(const int nPort,const int nBaudRate,const int nDataBits,const int nStopBits,BOOL bIfParity,const int nParity,IConnection**ppConnetion);
	
	//向连接中发送数据;
	//byBuffer，待发送的数据缓冲数组；
	//nDataLen，待发送的数据字节数；
	//返回值，完成写入的字节个数；
	virtual int  WriteData(const unsigned char byBuffer[],const int nDataLen);
	
	//从连接中读取数据
	//byBuffer,接收数据缓冲区；
	//nBufferLen,接收数据缓冲区的长度，in Bytes
	//nReadLen，成功读取的字节个数。
	//rtn,操作成功:返回1;失败：0；
	virtual BOOL ReadData (unsigned char byBuffer[], const int  nBufferLen,int &nReadLen);
	
	//判断连接是否有效;
	//返回值，有效：1;无效：0；
	virtual BOOL Valid()const;
	
	//关闭连接,释放连接资源。
	//返回值，无；
	virtual void Release();
	
protected:
	//设置串口的参数信息；
	void EnableParity(const BOOL  bIfParity);
	void SetParity   (const WORD  byMask);
	void SetDataBits (const  int  nNum);
	void SetStopBits (const  int  nNum);
    void SetPortNo   (const  int  nPortNo);
	void SetBaudRate (const  int  nBaudRate);
    
    //设置BCD状态；
	BOOL InitDCB();
    
	//设置收取数据和发送数据的超时参数。
	BOOL InitCommTimeouts();
	
	//打开串口操作；
	BOOL OpenPort();

    //常量，串口双向缓冲区的大小；
	enum{COMQUESIZE = 2048};
private:
	//数据参数区
	int    m_nPortNo;
	int    m_nBaudRate;
	HANDLE m_hFile;
	DWORD  m_dwReadLen;
	BOOL   m_bifParity;
	WORD   m_wParity;
	int    m_nDataBits;
	int    m_nStopBits;	
	basic_string<TCHAR> m_strDescription;
    IConnection * m_pTransConnection;
};
