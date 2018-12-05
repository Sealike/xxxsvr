#pragma once
#include <afxmt.h>
#include <string>
#include <list>
#include <vector>

using namespace std;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class  ILocker
{
protected:
	ILocker(void){;}
	~ILocker(void){;}
public:
	//����
	virtual void Lock()            = 0;
	//����
	virtual void Unlock()          = 0;
	//�����������
	virtual int  GetLockCnt()const = 0;
};

class CLockCriticalSection :public ILocker
{
public:
	CLockCriticalSection(void) ;
	~CLockCriticalSection(void);
	//����
	virtual void Lock  ();
	//����
	virtual void Unlock();
	//�����������
	virtual int  GetLockCnt()const ;
	
private:
	//��������
	int m_nLockCnt;
	//��������
	CCriticalSection m_cs;
};

//�����ض���
typedef CLockCriticalSection CLockCs;

class  CLockNone : public ILocker
{
public:
	CLockNone(void);
	~CLockNone(void);
	
	//����
	virtual void Lock  ();
	//����
	virtual void Unlock();
	//�����������
	virtual int  GetLockCnt()const ;
private:
	//��������
	int m_nLockCnt;
};

//����һ����ջ��ʹ��, ʹ��ʱҪС�ķ�����ȫ�ԡ�
//���಻��������������������Ӱ�촫�����������

class CSmartLock
{
public:
	//���캯��
	//pLocker,ʵ�����������ָ�룻
	//�ڶ��󹹽�ʱ���Զ�������
	CSmartLock(ILocker * pLocker)
	{
		m_pLock = pLocker; 
		if(m_pLock != NULL) 
			m_pLock->Lock();
	}
	
	//��������
	//�������������Զ�������
	~CSmartLock(void)
	{
		if(m_pLock != NULL) 
			m_pLock->Unlock();
	}
    
private:
	//����ʵ�ֶ���ָ��
	ILocker * m_pLock;
};

#define  AUTOLOCK(cs) \
		 CSmartLock Auto(&cs);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ͳһ��Ϣ���ͽӿڶ���
class IMsgData
{
protected:
	IMsgData(void);
	~IMsgData(void);
	
public:
	virtual IMsgData   *  Copy ()const = 0;
	virtual int           GetId()const = 0;
	virtual const TCHAR*  GetDescription()const = 0;
	virtual void       *  GetMsgPtr()const = 0;
	virtual int           GetType()const = 0;
	virtual void          Release() = 0;
	virtual BOOL          ifDelegated() = 0;	
	virtual void          EnableDelegate(BOOL nEnable) = 0;
};

//ͳһ��Ϣ�ඨ��
//ģ�����T :��Ϣ�ṹ�����͡�
template<typename T>
class CUniversalMsg : public IMsgData  
{
	CUniversalMsg(){m_bDelegated = FALSE ;}
	virtual ~CUniversalMsg(){;}
	
public:
	//ͳһ���Ͷ���
	typedef  T*  TypePtr;
	typedef  T   Type;
	
	typedef  const T* constPtr;
    
	//������Ϣ�Ĳ���.�೧����������
	static CUniversalMsg<T>* Alloc(int nMsgId, const TCHAR * pstrName, constPtr pMsg);
	//��¡һ�ݣ�
	virtual IMsgData*        Copy()const 
	{	
		IMsgData* pMsgData = CUniversalMsg<Type>::Alloc(nMsgId,charDescr.c_str(),&(Data.Msg));
		if(pMsgData != NULL)
			pMsgData->EnableDelegate(TRUE);
		
		return pMsgData;
	}
	//�õ���Ϣ�ı�ʶID
	virtual int              GetId()const{return nMsgId;}
	//�����Ϣ�������ַ���
	virtual const TCHAR*     GetDescription()const{return charDescr.c_str();}
	//�����Ϣ��ָ��
	virtual void    *        GetMsgPtr()const{return (void*)&(Data.Msg);};
	//�����Ϣ�����ͣ���ʱ�����塣
	virtual int              GetType()const{return nMsgId;}
	//�ͷ���Ϣ��Դ��Ψһ���á�
	virtual void             Release(){delete this;}

	//�жϸ���Ϣ�Ƿ�ί�и��˿ͻ��˽��й���
	//TRUE:ָʾ�ͻ�����ȫӵ�и���Ϣ�����ձ�������ͷţ�
	//FALSE:ָʾ�ͻ���û��ӵ�и���Ϣ�����ܽ����ͷŲ�����
	virtual BOOL             ifDelegated(){return m_bDelegated;}	

	//���ø���Ϣ�Ƿ�Ϊί��״̬
	virtual void             EnableDelegate(BOOL nEnable){m_bDelegated = nEnable;}
	
protected:
	
	CUniversalMsg (int nMsgId, const TCHAR * pstrName, constPtr pMsg);
	
private:
	BOOL  m_bDelegated;
	//��ϢID
	int   nMsgId;
	//�����ַ���
	basic_string<TCHAR>charDescr;
	//��Ϣ�Ĵ洢�ռ䶨�塣
	union{
		char  Buffer[sizeof(Type)];
		Type  Msg;
	}Data;		
};

template<typename T>
CUniversalMsg<T>* CUniversalMsg<T>::Alloc(int nMsgId, const TCHAR * pstrName, constPtr pMsg)
{
	CUniversalMsg<T> * p = new CUniversalMsg<T>(nMsgId,pstrName,pMsg);	
	return p;
}

template<typename T>
CUniversalMsg<T>::CUniversalMsg(int nMsgId, const TCHAR * pstrName, constPtr pMsg)
{   
	charDescr = pstrName;
	this->nMsgId = nMsgId;
	memcpy(&(Data.Msg),pMsg,sizeof(T));
};

//������������������ IMsgData;
template<typename T>
IMsgData* MakeMsg(int nMsgId, const TCHAR * pstrName, const T* pData)
{	
	CUniversalMsg<T>* pMsg = CUniversalMsg<T>::Alloc(nMsgId, pstrName, pData);
	return pMsg;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//�ͻ���֪ͨǶ��ӿ����Ͷ��塣
class  CSinker{
public:
	virtual void  Update(const UINT uMsgId,const void * pMsgBuffer,int para) = 0;
	virtual void  Log(const char * pStrLogInfo) = 0;
};

//�����Զ�����չЭ��ص�����
typedef int (*pCBExt_explain)(const char* frame,int framelen,BYTE *DataBuff, DWORD &Len);

//�����֪ͨ��Ϣ���ͽӿڶ���
class IMessager
{
public:
	virtual int  Advise  (CSinker* pSinker) = 0 ;
	virtual int  Unadvise(const CSinker* pSinker) = 0 ;
};

class CMessager :public IMessager
{
public:
	virtual int  Advise  (CSinker* pSinker) ;
	virtual int  Unadvise(const CSinker* pSinker);
protected:
	void NotifyMsg(int msgid, char* msg ,int msglen);
private:
	vector<CSinker*> m_sinkers;
};

class ISearchExplainer{
public:
	virtual int  ProcessData(const BYTE byBuffData[],const int nDataLen) = 0 ;
	virtual int  EncodeFrame(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen) = 0;
	virtual void Release() = 0;
	virtual BOOL AddExtenedPlugin(int msgid,char* title,pCBExt_explain exp_cb)= NULL;
};

//Э�����ݴ���ӿ����Ͷ���
class  CSearchExplainer :public ISearchExplainer,public CMessager
{ 
public:
	virtual int  ProcessData(const BYTE byBuffData[],const int nDataLen) = 0 ;
	virtual int  EncodeFrame(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen) = 0;
	virtual void Release() = 0;
	virtual BOOL AddExtenedPlugin(int msgid,char* title,pCBExt_explain exp_cb) {return FALSE;}	
};

typedef CSinker          ISinker;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//�������ӽӿ����Ͷ��塣
class IConnection
{

protected:
	virtual ~IConnection(void);
	IConnection(void);
	
public:
	//�������з������ݣ�
	//byBuffer�������͵����ݻ������飻
	//nDataLen�������͵������ֽ�����
	//����ֵ�����д����ֽڸ�����
	virtual int  WriteData(const unsigned char byBuffer[],const int nDataLen) = 0;
	
	//�������ж�ȡ����
	//byBuffer,�������ݻ�������
	//nBufferLen,�������ݻ������ĳ��ȣ�in Bytes
	//nReadLen���ɹ���ȡ���ֽڸ�����
	//rtn,�����ɹ�:����1;ʧ�ܣ�0��
	virtual BOOL ReadData(unsigned char buff[],const int nBuffLen,int & nReadLen) = 0;
	
	//����ת�����ӣ�
	//pConnection,Ҫ���õ�ת�����ӣ�
	//���أ���ǰ��ת�����ӣ�
	//Ҫ����ת������ʱ����pConnection����ΪNULL;
	virtual IConnection* TransitConnect(IConnection * pConnection) = 0;

	//�ж������Ƿ���Ч
	//����ֵ����Ч��1;��Ч��0��
	virtual BOOL Valid()const = 0 ;
	
	//�ر�����,�ͷ�������Դ��
	//����ֵ���ޣ�
	virtual void Release() = 0;

	//�������������ַ���
	//strDescrip,�û������ַ�����
	//����ֵ���ޣ�
	virtual void SetDescript(const TCHAR* strDescript) = 0;

	//��ȡ���������ַ���
	//����ֵ���û������ַ��������÷���ø��ַ����󣬲�Ҫ�޸ģ�
	virtual	const TCHAR* GetDescript()const = 0;
	
};

/* ����δ�������Ժ�Ӧ��
   �����跨�ṩ�̰߳�ȫ���̲߳���ȫʵ�֡�
   TLockerģ�����������ʵ����ILocker�ӿڵ��࣬��CSafeConnection���л�Ĭ�Ϸ���ILock�ӿڡ�
   ���಻���д����IConnection����ָ�롣CSafeConnection<T>��������������Ӱ�촫���IConnection����,������ʾ����CSafeConnection<T>������Realse�����ͷ�����
   ��ʱʵ���˷��͵��̰߳�ȫ�ԡ��������ʲ�����ȫ�Կ���������ӡ�
*/

template<typename TLocker>
class CSafeConnection :
	public IConnection
{
protected:
	virtual ~CSafeConnection(void){;}
public:
	CSafeConnection(IConnection* pConnection){m_pConnection = pConnection;}
	
	
	//�������з������ݣ�
	//byBuffer�������͵����ݻ������飻
	//nDataLen�������͵������ֽ�����
	//����ֵ�����д����ֽڸ�����
	virtual int  WriteData(const unsigned char byBuffer[],const int nDataLen);
	
	//�������ж�ȡ����
	//byBuffer,�������ݻ�������
	//nBufferLen,�������ݻ������ĳ��ȣ�in Bytes
	//nReadLen���ɹ���ȡ���ֽڸ�����
	//rtn,�����ɹ�:����1;ʧ�ܣ�0��
	virtual BOOL ReadData(unsigned char buff[],const int nBuffLen,int & nReadLen);
	
	//����ת�����ӣ�
	//pConnection,Ҫ���õ�ת�����ӣ�
	//���أ���ǰ��ת�����ӣ�
	//Ҫ����ת������ʱ����pConnection����ΪNULL;
	virtual IConnection* TransitConnect(IConnection * pConnection);

	//�ж������Ƿ���Ч
	//����ֵ����Ч��1;��Ч��0��
	virtual BOOL Valid()const;
	
	//�ر�����,�ͷ�������Դ��
	//����ֵ���ޣ�
	virtual void Release();
	
	//�������������ַ���
	//strDescrip,�û������ַ�����
	//����ֵ���ޣ�
	virtual void SetDescript(const TCHAR* strDescript);
	
	//����ڿ��豸���ָ�붨λ������
	//nPos������ָ���λ�ã�
	//nRef, �ο�λ�ã�
	//���أ����óɹ���TRUE;ʧ��:FALSE;
	virtual int  Seek       (const int   nPos , const int nRef);
	
	//��ȡ���������ַ���
	//����ֵ���û������ַ��������÷���ø��ַ����󣬲�Ҫ�޸ģ�
	virtual const TCHAR* GetDescript()const ;
	
private:
	IConnection * m_pConnection;
	TLocker       m_LockerSnd;
};

template<typename TLocker>
int  CSafeConnection<TLocker>::WriteData(const unsigned char byBuffer[],const int nDataLen)
{	
	AUTOLOCK(m_LockerSnd);
	if(byBuffer == NULL || nDataLen == 0)
		return 0;

	if(m_pConnection != NULL)
	{
		return m_pConnection->WriteData(byBuffer,nDataLen);
	}else
		return 0;
}

template<typename TLocker>
BOOL CSafeConnection<TLocker>::ReadData(unsigned char buff[],const int nBuffLen,int & nReadLen)
{   
	if(buff == NULL || nBuffLen == 0)
		return FALSE;

	if(m_pConnection != NULL)
	{	
		m_pConnection->ReadData(buff,nBuffLen,nReadLen);
	}else
		return FALSE;
}

template<typename TLocker>
IConnection* CSafeConnection<TLocker>::TransitConnect(IConnection * pConnection)
{   
	if(m_pConnection != NULL)
	{   
		return m_pConnection->TransitConnect(pConnection);
	}else
		return NULL;
}

//�ж������Ƿ���Ч
//����ֵ����Ч��1;��Ч��0��
template<typename TLocker>
BOOL CSafeConnection<TLocker>::Valid()const
{   
	BOOL bValid = FALSE;
	
	if(m_pConnection != NULL)
	{   
		bValid = m_pConnection->Valid();
	}

	return bValid;
}

template<typename TLocker>
void CSafeConnection<TLocker>::Release()
{
	AUTOLOCK(m_LockerSnd);
	
	if(m_pConnection != NULL)
	{   
		m_pConnection->Release();
		m_pConnection = NULL;
	}

	delete this;
}

//�������������ַ���
//strDescrip,�û������ַ�����
//����ֵ���ޣ�
template<typename TLocker>
void CSafeConnection<TLocker>::SetDescript(const TCHAR* strDescript)
{   
	if(m_pConnection != NULL && strDescript != NULL)
	{   
		m_pConnection->SetDescript(strDescript);
	}
}

//����ڿ��豸���ָ�붨λ������
//nPos������ָ���λ�ã�
//nRef, �ο�λ�ã�
//���أ����óɹ���TRUE;ʧ��:FALSE;
template<typename TLocker>
int  CSafeConnection<TLocker>::Seek       (const int   nPos , const int nRef)
{
	if(m_pConnection != NULL)
	{
		return m_pConnection->Seek(nPos,nRef);
	}else
		return 0;
}

//��ȡ���������ַ���
//����ֵ���û������ַ��������÷���ø��ַ����󣬲�Ҫ�޸ģ�
template<typename TLocker>
const TCHAR* CSafeConnection<TLocker>::GetDescript()const 
{
	if(m_pConnection != NULL)
	{
		return m_pConnection->GetDescript();
	}else
		return _T("");
}

//�����������������̡߳�д����ȫ�����ӣ�
template<typename T>  
IConnection* MakeSafeConnection(IConnection * pConnection)
{
	CSafeConnection<T> * pConnection = new CSafeConnection<T>(pConnection);
	return pConnection;
}

//�۲��߶��塣��
class CObserver  
{
public:
    
	CObserver();
	virtual ~CObserver();
    //////////////////////////////////////////////////////////////////////////    	
	virtual BOOL Update(IMsgData *pInfo, UINT uInfoID, WPARAM wPara,LPARAM lPara,int nMsgLen);    
	
};

typedef   CObserver   IObserver;

//����۲��߶�����
class CUiObserver : public CObserver  
{   
public:
	CUiObserver();
	virtual       ~CUiObserver();
	//���ý��漰֪ͨ��Ϣ
	virtual void   SetNotifyInfo(HWND hWnd,DWORD dwMsgId);
	virtual BOOL   Update(IMsgData * pInfo, UINT uInfoID, WPARAM wPara,LPARAM lPara,int nMsgLen);
    
private:
	HWND   m_hWnd;//������
	DWORD  m_dwMsgId;//֪ͨ��Ϣ��ʶ
	list<IMsgData *>          m_lsProtocalData;//not used!
	CLockCriticalSection      m_csList;
};

//���ɹ۲��߶�����
class CFreeObserver : public CObserver  
{
public:
	CFreeObserver();
	virtual ~CFreeObserver();
    
	virtual BOOL Update(IMsgData *pInfo, UINT uInfoID, WPARAM wPara,LPARAM lPara,int nMsgLen);    
protected:
	CLockCriticalSection m_csUpdate;
};

//���۲��߶���,
//���Խ���һ�Զ���Ϣ֪ͨ��
class CObservedObj  
{
public:
	CObservedObj();
	virtual ~CObservedObj();
	
	virtual void Attatch(CObserver *pObserver);
	virtual void Detatch(CObserver *pObserver);
protected:
	//�����ImsgData���ͺ�Ӧ�üܹ��󶨵��е������
	virtual BOOL Notify(IMsgData *pMsg,int nMsgType,WPARAM wParam,LPARAM lParam,int nLen);    
    
protected:
	list<CObserver *> m_listObserver;
	CLockCriticalSection m_csObservers;
};

typedef CObservedObj   IObservedObj;

//����ܵ�����
class CPipe : public CObserver ,public  CObservedObj
{
public:
	CPipe(void);
	virtual ~CPipe(void);
protected:

};

typedef CPipe IPipe;

#define   __BD       0
#define   __GPS      1
#define   __GLO      2
#define   __GAL      3
#define   __GNSS     4

#define   __B1       0
#define   __B2       1
#define   __B3       2
#define   __L1       3

#define   __PI       3.141592653589793