#pragma once
#include "GeneralConnection.h"

/*
���߱����߳����ԣ�
û��Ӧ�þ�����
û�в��ԣ�
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

	//�������Ӻ���...
	//nPort��Ŀ��˿ں�
	//nBaudRate,�򿪴��ڵĲ����ʣ�
	//nDataBits,����λ�ĸ�����
	//nStopBits,ֹͣλ�ĸ�����
	//nParity�� У��λ�ķ�ʽ��
	//ppConnetion���������������������ָ�룻
	//����ֵ�������ɹ�������TRUE,���򣺷���FALSE��
	static BOOL  OpenAConnection(const int nPort,const int nBaudRate,const int nDataBits,const int nStopBits,BOOL bIfParity,const int nParity,IConnection**ppConnetion);
	
	//�������з�������;
	//byBuffer�������͵����ݻ������飻
	//nDataLen�������͵������ֽ�����
	//����ֵ�����д����ֽڸ�����
	virtual int  WriteData(const unsigned char byBuffer[],const int nDataLen);
	
	//�������ж�ȡ����
	//byBuffer,�������ݻ�������
	//nBufferLen,�������ݻ������ĳ��ȣ�in Bytes
	//nReadLen���ɹ���ȡ���ֽڸ�����
	//rtn,�����ɹ�:����1;ʧ�ܣ�0��
	virtual BOOL ReadData (unsigned char byBuffer[], const int  nBufferLen,int &nReadLen);
	
	//�ж������Ƿ���Ч;
	//����ֵ����Ч��1;��Ч��0��
	virtual BOOL Valid()const;
	
	//�ر�����,�ͷ�������Դ��
	//����ֵ���ޣ�
	virtual void Release();
	
protected:
	//���ô��ڵĲ�����Ϣ��
	void EnableParity(const BOOL  bIfParity);
	void SetParity   (const WORD  byMask);
	void SetDataBits (const  int  nNum);
	void SetStopBits (const  int  nNum);
    void SetPortNo   (const  int  nPortNo);
	void SetBaudRate (const  int  nBaudRate);
    
    //����BCD״̬��
	BOOL InitDCB();
    
	//������ȡ���ݺͷ������ݵĳ�ʱ������
	BOOL InitCommTimeouts();
	
	//�򿪴��ڲ�����
	BOOL OpenPort();

    //����������˫�򻺳����Ĵ�С��
	enum{COMQUESIZE = 2048};
private:
	//���ݲ�����
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
