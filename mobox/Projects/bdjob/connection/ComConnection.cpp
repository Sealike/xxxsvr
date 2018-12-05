#include "ComConnection.h"
//////////////////////////////////////////////////////////////////////////
//通用连接实现文件
//created
// by lxl
//2013-09-09
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//constructor
//////////////////////////////////////////////////////////////////////////
CComConnection::CComConnection(void)
{   
	m_nPortNo   = 0;
	m_nBaudRate = 0;
	m_hFile     = INVALID_HANDLE_VALUE;
	m_dwReadLen = 0;
	m_wParity   = 0;
	m_nDataBits = 0;
	m_nStopBits = 0;
	m_bifParity = FALSE;
	m_strDescription = _T("com connection...");
	m_pTransConnection = NULL;
}

CComConnection::~CComConnection(void)
{
	
}

////////////////////////////////////////////////////////////////////////////////
//打开一个连接
////////////////////////////////////////////////////////////////////////////////
BOOL CComConnection::OpenAConnection(const int nPort,const int nBaudRate,const int nDataBits,const int nStopBits,BOOL bIfParity,const int nParity,IConnection**ppConnection)
{   
	BOOL bOpen = FALSE ;
	CComConnection* pConnection = new CComConnection();
	
	if(NULL != pConnection)
	{		
		pConnection->EnableParity(bIfParity);
		pConnection->SetParity   (nParity);
		pConnection->SetStopBits (nStopBits);
		pConnection->SetDataBits (nDataBits);
		pConnection->SetPortNo   (nPort);
		pConnection->SetBaudRate (nBaudRate);	
		
		bOpen = pConnection->OpenPort();
 
		if(bOpen)
		{			
			*ppConnection = pConnection;
		}else{
			*ppConnection = NULL;
			delete pConnection;
		}
	}

	return bOpen;
}

////////////////////////////////////////////////////////////////////////////////
//设置端口
//nPortNo
////////////////////////////////////////////////////////////////////////////////
void CComConnection::SetPortNo  (const int nPortNo)
{
	m_nPortNo = nPortNo;
}

////////////////////////////////////////////////////////////////////////////////
//设置端口
//nBaudRate
////////////////////////////////////////////////////////////////////////////////
void CComConnection::SetBaudRate(const int nBaudRate)
{
	m_nBaudRate = nBaudRate;
}

////////////////////////////////////////////////////////////////////////////////
//设置端口
//nBaudRate
////////////////////////////////////////////////////////////////////////////////
void CComConnection::SetParity(const WORD byMask)
{
	m_wParity = byMask;
}

////////////////////////////////////////////////////////////////////////////////
//设置端口
//nBaudRate
////////////////////////////////////////////////////////////////////////////////
void CComConnection::EnableParity(const BOOL bIfParity)
{
	m_bifParity = bIfParity;
}

////////////////////////////////////////////////////////////////////////////////
//设置端口
//nBaudRate
////////////////////////////////////////////////////////////////////////////////
void CComConnection::SetDataBits(const int nNum)
{
	m_nDataBits = nNum;
}

////////////////////////////////////////////////////////////////////////////////
//设置端口
//nBaudRate
////////////////////////////////////////////////////////////////////////////////
void CComConnection::SetStopBits(const int nNum)
{
	m_nStopBits = nNum;
}

////////////////////////////////////////////////////////////////////////////////
//有效操作
//nBaudRate
////////////////////////////////////////////////////////////////////////////////
BOOL CComConnection::Valid()const
{
	BOOL bRtn = FALSE;
	
	if(m_hFile!=INVALID_HANDLE_VALUE && m_hFile != NULL)
		bRtn =TRUE;

	return bRtn;
}

////////////////////////////////////////////////////////////////////////////////
//打开操作
//nBaudRate
////////////////////////////////////////////////////////////////////////////////
BOOL CComConnection::OpenPort()
{
	BOOL    bRtn      = FALSE;
	CString strComName;
    
	if(Valid())
	{
		PurgeComm(m_hFile, PURGE_TXCLEAR | PURGE_RXCLEAR);
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
	//strComName.Format(_T("COM%d:"),m_nPortNo);
	strComName.Format(_T("\\\\.\\COM%d"),m_nPortNo);//lxl 2016-03-04
	m_hFile = CreateFile(strComName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if(m_hFile!=INVALID_HANDLE_VALUE)
	{
		SetCommMask (m_hFile, EV_RXCHAR | EV_CTS | EV_DSR | EV_RING);

		//if(SetupComm(m_hFile, COMQUESIZE, COMQUESIZE))	
		//{
		if(InitDCB())
		{
			if(InitCommTimeouts())
			{
				bRtn = TRUE;
			}
		}
		//}
		
		if(!bRtn)
		{
			CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
		}
	}

	return bRtn ;
}

BOOL CComConnection::InitDCB()
{   
    DCB fDcb;
	
	memset(&fDcb,0,sizeof(DCB));
	fDcb.DCBlength = sizeof (DCB);
	if(!GetCommState (m_hFile, &fDcb))
		return FALSE;
    
	fDcb.fBinary = TRUE; //Win32不支持非二进制串行传输模式，必须为TRUE 
	fDcb.fParity = m_bifParity; //启用奇偶校验
	//	fDcb.fOutxCtsFlow = TRUE; //串行端口的输出由CTS线控制
	//	fDcb.fOutxDsrFlow = TRUE; //串行端口的输出由DSR线控制
	fDcb.fOutxCtsFlow = FALSE; //禁止CTS线来控制端口的输出
	fDcb.fOutxDsrFlow = FALSE; //禁止DSR线来控制端口的输出
	fDcb.fDtrControl  = DTR_CONTROL_ENABLE; //启用DTR线
	fDcb.fDsrSensitivity = FALSE; //如果设为TRUE将忽略任何输入的字节，除非DSR线被启用 
	fDcb.fTXContinueOnXoff = FALSE; //当为TRUE时，如果接收缓冲区已满且驱动程序已传送XOFF字符，将使驱动程序停止传输字符
	fDcb.fOutX = FALSE; //设为TRUE指定XON/XOFF控制被用于控制串行输出
	fDcb.fInX = FALSE; //设为TRUE指定XON/XOFF控制被用于控制串行输入
	fDcb.fNull = FALSE; //设为TRUE将使串行驱动程序忽略收到的空字节
	fDcb.fRtsControl = RTS_CONTROL_ENABLE; //启用RTS线
	//	fDcb.ErrorChar = 63; //WINCE串行驱动程序的默认执行将忽略这个字段
	//	fDcb.fErrorChar = FALSE; //WINCE串行驱动程序的默认执行将忽略这个字段
	//	fDcb.fAbortOnError = FALSE; //WINCE串行驱动程序的默认执行将忽略这个字段
	
	fDcb.BaudRate = m_nBaudRate; //波特率
	fDcb.ByteSize = (BYTE)m_nDataBits; //每字节的位数
	fDcb.Parity   = (BYTE)m_wParity;//PARITY_ODD >> 4;//;  //无奇偶校验 
	fDcb.StopBits = (BYTE)m_nStopBits;//; //每字节一位停止位
	
	//根据DCB结构配置端口
	if (!SetCommState (m_hFile, &fDcb))
		return FALSE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//InitCommTimeouts
//nBaudRate
////////////////////////////////////////////////////////////////////////////////
BOOL CComConnection::InitCommTimeouts()
{
   	COMMTIMEOUTS fTimemOut;
	
	memset(&fTimemOut,0,sizeof(COMMTIMEOUTS));
	GetCommTimeouts (m_hFile, &fTimemOut);
	
	fTimemOut.ReadIntervalTimeout = 100; //ms
	fTimemOut.ReadTotalTimeoutMultiplier = 1;//ms
	fTimemOut.ReadTotalTimeoutConstant = 10;
	
	fTimemOut.WriteTotalTimeoutMultiplier = 10;
	fTimemOut.WriteTotalTimeoutConstant = 1000;
	
	if (!SetCommTimeouts (m_hFile, &fTimemOut))
		return FALSE;
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//Release
//nBaudRate
////////////////////////////////////////////////////////////////////////////////
void CComConnection::Release()
{   
	if(Valid())
	{
		  PurgeComm(m_hFile, PURGE_TXCLEAR | PURGE_RXCLEAR);
		  CloseHandle(m_hFile);
		  m_hFile = INVALID_HANDLE_VALUE;
	}
	
	delete this;
}

////////////////////////////////////////////////////////////////////////////////
//ReadData
//nBaudRate
////////////////////////////////////////////////////////////////////////////////
BOOL CComConnection::ReadData (unsigned char byBuffer[], const int  nBufferLen,int &nReadLen)
{   
	BOOL    bRtn      = FALSE;
	DWORD   dwErrors  =  0;
	COMSTAT ComStatus = {0};    	
    
	nReadLen = 0;
	
	if(Valid())
	{   
		DWORD nOnceRead = 0; //每次读取的字节数；
		BOOL  OK = ClearCommError(m_hFile, &dwErrors, &ComStatus); //获取串口状态。   
        
		if(ComStatus.cbInQue > 0)
		{   
			if(ReadFile(m_hFile, byBuffer,nBufferLen-1, (DWORD*)&nOnceRead, NULL))//block call
			{   
				nReadLen = nOnceRead;
				bRtn     = TRUE;
				TransitSend(byBuffer,nOnceRead);	
				byBuffer[nReadLen] = '\0';
				/*TRACE("%s",(char*)byBuffer);*/
				TRACE(_T("%s"),_T("read1 ..\r\n"));
			}
			
		}else
		{   
			Sleep(10);//暂时放弃CPU;
			TRACE(_T("%s"),_T("read2 ..\r\n"));
			return TRUE;
		}		
	}
	
	return bRtn;
}


////////////////////////////////////////////////////////////////////////////////
//WriteData
//nBaudRate
//df
////////////////////////////////////////////////////////////////////////////////
int  CComConnection::WriteData(const unsigned char byBuffer[],const int nDataLen)
{	
	int nSumWrtn = 0;

	if(Valid())
	{   
		DWORD dwWtn   = 0;
		int   nWrtLen = nDataLen;//要写入的字节数；
		//写入数据
		while(nWrtLen>0)//%
		{	
			if(!WriteFile(m_hFile, byBuffer+nSumWrtn , nWrtLen, &dwWtn, NULL))//阻塞写文件。
			{
				DWORD f_dwErrors = GetLastError();
				if(f_dwErrors != ERROR_IO_PENDING)//写入出现错误；
					break;
			}else if(dwWtn == 0)
			{
				break;
			}else{
				nSumWrtn += dwWtn;//进行计数；
				nWrtLen -= dwWtn ;		
			}
		}
	}
    
	return nSumWrtn;
}
