#include "ComConnection.h"
//////////////////////////////////////////////////////////////////////////
//ͨ������ʵ���ļ�
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
//��һ������
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
//���ö˿�
//nPortNo
////////////////////////////////////////////////////////////////////////////////
void CComConnection::SetPortNo  (const int nPortNo)
{
	m_nPortNo = nPortNo;
}

////////////////////////////////////////////////////////////////////////////////
//���ö˿�
//nBaudRate
////////////////////////////////////////////////////////////////////////////////
void CComConnection::SetBaudRate(const int nBaudRate)
{
	m_nBaudRate = nBaudRate;
}

////////////////////////////////////////////////////////////////////////////////
//���ö˿�
//nBaudRate
////////////////////////////////////////////////////////////////////////////////
void CComConnection::SetParity(const WORD byMask)
{
	m_wParity = byMask;
}

////////////////////////////////////////////////////////////////////////////////
//���ö˿�
//nBaudRate
////////////////////////////////////////////////////////////////////////////////
void CComConnection::EnableParity(const BOOL bIfParity)
{
	m_bifParity = bIfParity;
}

////////////////////////////////////////////////////////////////////////////////
//���ö˿�
//nBaudRate
////////////////////////////////////////////////////////////////////////////////
void CComConnection::SetDataBits(const int nNum)
{
	m_nDataBits = nNum;
}

////////////////////////////////////////////////////////////////////////////////
//���ö˿�
//nBaudRate
////////////////////////////////////////////////////////////////////////////////
void CComConnection::SetStopBits(const int nNum)
{
	m_nStopBits = nNum;
}

////////////////////////////////////////////////////////////////////////////////
//��Ч����
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
//�򿪲���
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
    
	fDcb.fBinary = TRUE; //Win32��֧�ַǶ����ƴ��д���ģʽ������ΪTRUE 
	fDcb.fParity = m_bifParity; //������żУ��
	//	fDcb.fOutxCtsFlow = TRUE; //���ж˿ڵ������CTS�߿���
	//	fDcb.fOutxDsrFlow = TRUE; //���ж˿ڵ������DSR�߿���
	fDcb.fOutxCtsFlow = FALSE; //��ֹCTS�������ƶ˿ڵ����
	fDcb.fOutxDsrFlow = FALSE; //��ֹDSR�������ƶ˿ڵ����
	fDcb.fDtrControl  = DTR_CONTROL_ENABLE; //����DTR��
	fDcb.fDsrSensitivity = FALSE; //�����ΪTRUE�������κ�������ֽڣ�����DSR�߱����� 
	fDcb.fTXContinueOnXoff = FALSE; //��ΪTRUEʱ��������ջ��������������������Ѵ���XOFF�ַ�����ʹ��������ֹͣ�����ַ�
	fDcb.fOutX = FALSE; //��ΪTRUEָ��XON/XOFF���Ʊ����ڿ��ƴ������
	fDcb.fInX = FALSE; //��ΪTRUEָ��XON/XOFF���Ʊ����ڿ��ƴ�������
	fDcb.fNull = FALSE; //��ΪTRUE��ʹ����������������յ��Ŀ��ֽ�
	fDcb.fRtsControl = RTS_CONTROL_ENABLE; //����RTS��
	//	fDcb.ErrorChar = 63; //WINCE�������������Ĭ��ִ�н���������ֶ�
	//	fDcb.fErrorChar = FALSE; //WINCE�������������Ĭ��ִ�н���������ֶ�
	//	fDcb.fAbortOnError = FALSE; //WINCE�������������Ĭ��ִ�н���������ֶ�
	
	fDcb.BaudRate = m_nBaudRate; //������
	fDcb.ByteSize = (BYTE)m_nDataBits; //ÿ�ֽڵ�λ��
	fDcb.Parity   = (BYTE)m_wParity;//PARITY_ODD >> 4;//;  //����żУ�� 
	fDcb.StopBits = (BYTE)m_nStopBits;//; //ÿ�ֽ�һλֹͣλ
	
	//����DCB�ṹ���ö˿�
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
		DWORD nOnceRead = 0; //ÿ�ζ�ȡ���ֽ�����
		BOOL  OK = ClearCommError(m_hFile, &dwErrors, &ComStatus); //��ȡ����״̬��   
        
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
			Sleep(10);//��ʱ����CPU;
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
		int   nWrtLen = nDataLen;//Ҫд����ֽ�����
		//д������
		while(nWrtLen>0)//%
		{	
			if(!WriteFile(m_hFile, byBuffer+nSumWrtn , nWrtLen, &dwWtn, NULL))//����д�ļ���
			{
				DWORD f_dwErrors = GetLastError();
				if(f_dwErrors != ERROR_IO_PENDING)//д����ִ���
					break;
			}else if(dwWtn == 0)
			{
				break;
			}else{
				nSumWrtn += dwWtn;//���м�����
				nWrtLen -= dwWtn ;		
			}
		}
	}
    
	return nSumWrtn;
}
