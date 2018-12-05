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
	//锁定
	virtual void Lock()            = 0;
	//解锁
	virtual void Unlock()          = 0;
	//获得锁定计数
	virtual int  GetLockCnt()const = 0;
};

class CLockCriticalSection :public ILocker
{
public:
	CLockCriticalSection(void) ;
	~CLockCriticalSection(void);
	//锁定
	virtual void Lock  ();
	//解锁
	virtual void Unlock();
	//获得锁定计数
	virtual int  GetLockCnt()const ;
	
private:
	//锁定计数
	int m_nLockCnt;
	//加锁对象
	CCriticalSection m_cs;
};

//类型重定义
typedef CLockCriticalSection CLockCs;

class  CLockNone : public ILocker
{
public:
	CLockNone(void);
	~CLockNone(void);
	
	//锁定
	virtual void Lock  ();
	//解锁
	virtual void Unlock();
	//获得锁定计数
	virtual int  GetLockCnt()const ;
private:
	//锁定计数
	int m_nLockCnt;
};

//该类一般在栈上使用, 使用时要小心分析安全性。
//该类不持有锁定对象。其消亡不影响传入的锁定对象。

class CSmartLock
{
public:
	//构造函数
	//pLocker,实现锁定对象的指针；
	//在对象构建时，自动加锁；
	CSmartLock(ILocker * pLocker)
	{
		m_pLock = pLocker; 
		if(m_pLock != NULL) 
			m_pLock->Lock();
	}
	
	//析构函数
	//在析构函数中自动解锁；
	~CSmartLock(void)
	{
		if(m_pLock != NULL) 
			m_pLock->Unlock();
	}
    
private:
	//锁定实现对象指针
	ILocker * m_pLock;
};

#define  AUTOLOCK(cs) \
		 CSmartLock Auto(&cs);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//统一消息类型接口定义
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

//统一消息类定义
//模板参数T :消息结构体类型。
template<typename T>
class CUniversalMsg : public IMsgData  
{
	CUniversalMsg(){m_bDelegated = FALSE ;}
	virtual ~CUniversalMsg(){;}
	
public:
	//统一类型定义
	typedef  T*  TypePtr;
	typedef  T   Type;
	
	typedef  const T* constPtr;
    
	//产生消息的操作.类厂制作函数。
	static CUniversalMsg<T>* Alloc(int nMsgId, const TCHAR * pstrName, constPtr pMsg);
	//克隆一份；
	virtual IMsgData*        Copy()const 
	{	
		IMsgData* pMsgData = CUniversalMsg<Type>::Alloc(nMsgId,charDescr.c_str(),&(Data.Msg));
		if(pMsgData != NULL)
			pMsgData->EnableDelegate(TRUE);
		
		return pMsgData;
	}
	//得到消息的标识ID
	virtual int              GetId()const{return nMsgId;}
	//获得消息的描述字符串
	virtual const TCHAR*     GetDescription()const{return charDescr.c_str();}
	//获得消息的指针
	virtual void    *        GetMsgPtr()const{return (void*)&(Data.Msg);};
	//获得消息的类型，暂时无意义。
	virtual int              GetType()const{return nMsgId;}
	//释放消息资源的唯一调用。
	virtual void             Release(){delete this;}

	//判断该消息是否委托给了客户端进行管理
	//TRUE:指示客户端完全拥有该消息，最终必须进行释放；
	//FALSE:指示客户端没有拥有该消息，不能进行释放操作；
	virtual BOOL             ifDelegated(){return m_bDelegated;}	

	//设置该信息是否为委托状态
	virtual void             EnableDelegate(BOOL nEnable){m_bDelegated = nEnable;}
	
protected:
	
	CUniversalMsg (int nMsgId, const TCHAR * pstrName, constPtr pMsg);
	
private:
	BOOL  m_bDelegated;
	//消息ID
	int   nMsgId;
	//描述字符串
	basic_string<TCHAR>charDescr;
	//消息的存储空间定义。
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

//帮助函数；帮助创建 IMsgData;
template<typename T>
IMsgData* MakeMsg(int nMsgId, const TCHAR * pstrName, const T* pData)
{	
	CUniversalMsg<T>* pMsg = CUniversalMsg<T>::Alloc(nMsgId, pstrName, pData);
	return pMsg;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//客户端通知嵌入接口类型定义。
class  CSinker{
public:
	virtual void  Update(const UINT uMsgId,const void * pMsgBuffer,int para) = 0;
	virtual void  Log(const char * pStrLogInfo) = 0;
};

//厂家自定义扩展协议回调函数
typedef int (*pCBExt_explain)(const char* frame,int framelen,BYTE *DataBuff, DWORD &Len);

//服务端通知消息类型接口定义
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

//协议数据处理接口类型定义
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
//数据连接接口类型定义。
class IConnection
{

protected:
	~IConnection(void);
	IConnection(void);
	
public:
	//向连接中发送数据；
	//byBuffer，待发送的数据缓冲数组；
	//nDataLen，待发送的数据字节数；
	//返回值，完成写入的字节个数；
	virtual int  WriteData(const unsigned char byBuffer[],const int nDataLen) = 0;
	
	//从连接中读取数据
	//byBuffer,接收数据缓冲区；
	//nBufferLen,接收数据缓冲区的长度，in Bytes
	//nReadLen，成功读取的字节个数。
	//rtn,操作成功:返回1;失败：0；
	virtual BOOL ReadData(unsigned char buff[],const int nBuffLen,int & nReadLen) = 0;
	
	//设置转发连接，
	//pConnection,要设置的转发连接；
	//返回，当前的转发连接；
	//要禁用转发功能时，将pConnection设置为NULL;
	virtual IConnection* TransitConnect(IConnection * pConnection) = 0;

	//判断连接是否有效
	//返回值，有效：1;无效：0；
	virtual BOOL Valid()const = 0 ;
	
	//关闭连接,释放连接资源。
	//返回值，无；
	virtual void Release() = 0;

	//设置连接描述字符串
	//strDescrip,用户描述字符串；
	//返回值：无；
	virtual void SetDescript(const TCHAR* strDescript) = 0;

	//获取连接描述字符串
	//返回值：用户描述字符串；调用方获得该字符串后，不要修改；
	virtual	const TCHAR* GetDescript()const = 0;
	
};

/* 该类未经过测试和应用
   该类设法提供线程安全或线程不安全实现。
   TLocker模板参数必须是实现了ILocker接口的类，在CSafeConnection类中会默认访问ILock接口。
   该类不持有传入的IConnection对象指针。CSafeConnection<T>类对象的消亡不会影响传入的IConnection对象,但是显示调用CSafeConnection<T>类对象的Realse将会释放它。
   暂时实现了发送的线程安全性。其他访问操作安全性可以依此添加。
*/

template<typename TLocker>
class CSafeConnection :
	public IConnection
{
protected:
	virtual ~CSafeConnection(void){;}
public:
	CSafeConnection(IConnection* pConnection){m_pConnection = pConnection;}
	
	
	//向连接中发送数据；
	//byBuffer，待发送的数据缓冲数组；
	//nDataLen，待发送的数据字节数；
	//返回值，完成写入的字节个数；
	virtual int  WriteData(const unsigned char byBuffer[],const int nDataLen);
	
	//从连接中读取数据
	//byBuffer,接收数据缓冲区；
	//nBufferLen,接收数据缓冲区的长度，in Bytes
	//nReadLen，成功读取的字节个数。
	//rtn,操作成功:返回1;失败：0；
	virtual BOOL ReadData(unsigned char buff[],const int nBuffLen,int & nReadLen);
	
	//设置转发连接，
	//pConnection,要设置的转发连接；
	//返回，当前的转发连接；
	//要禁用转发功能时，将pConnection设置为NULL;
	virtual IConnection* TransitConnect(IConnection * pConnection);

	//判断连接是否有效
	//返回值，有效：1;无效：0；
	virtual BOOL Valid()const;
	
	//关闭连接,释放连接资源。
	//返回值，无；
	virtual void Release();
	
	//设置连接描述字符串
	//strDescrip,用户描述字符串；
	//返回值：无；
	virtual void SetDescript(const TCHAR* strDescript);
	
	//针对于块设备完成指针定位操作；
	//nPos，设置指针的位置；
	//nRef, 参考位置；
	//返回，设置成功：TRUE;失败:FALSE;
	virtual int  Seek       (const int   nPos , const int nRef);
	
	//获取连接描述字符串
	//返回值：用户描述字符串；调用方获得该字符串后，不要修改；
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

//判断连接是否有效
//返回值，有效：1;无效：0；
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

//设置连接描述字符串
//strDescrip,用户描述字符串；
//返回值：无；
template<typename TLocker>
void CSafeConnection<TLocker>::SetDescript(const TCHAR* strDescript)
{   
	if(m_pConnection != NULL && strDescript != NULL)
	{   
		m_pConnection->SetDescript(strDescript);
	}
}

//针对于块设备完成指针定位操作；
//nPos，设置指针的位置；
//nRef, 参考位置；
//返回，设置成功：TRUE;失败:FALSE;
template<typename TLocker>
int  CSafeConnection<TLocker>::Seek       (const int   nPos , const int nRef)
{
	if(m_pConnection != NULL)
	{
		return m_pConnection->Seek(nPos,nRef);
	}else
		return 0;
}

//获取连接描述字符串
//返回值：用户描述字符串；调用方获得该字符串后，不要修改；
template<typename TLocker>
const TCHAR* CSafeConnection<TLocker>::GetDescript()const 
{
	if(m_pConnection != NULL)
	{
		return m_pConnection->GetDescript();
	}else
		return _T("");
}

//辅助函数，创建受线程“写”安全的连接；
template<typename T>  
IConnection* MakeSafeConnection(IConnection * pConnection)
{
	CSafeConnection<T> * pConnection = new CSafeConnection<T>(pConnection);
	return pConnection;
}

//观察者定义。。
class CObserver  
{
public:
    
	CObserver();
	virtual ~CObserver();
    //////////////////////////////////////////////////////////////////////////    	
	virtual BOOL Update(IMsgData *pInfo, UINT uInfoID, WPARAM wPara,LPARAM lPara,int nMsgLen);    
	
};

typedef   CObserver   IObserver;

//界面观察者定义类
class CUiObserver : public CObserver  
{   
public:
	CUiObserver();
	virtual       ~CUiObserver();
	//设置界面及通知消息
	virtual void   SetNotifyInfo(HWND hWnd,DWORD dwMsgId);
	virtual BOOL   Update(IMsgData * pInfo, UINT uInfoID, WPARAM wPara,LPARAM lPara,int nMsgLen);
    
private:
	HWND   m_hWnd;//界面句柄
	DWORD  m_dwMsgId;//通知消息标识
	list<IMsgData *>          m_lsProtocalData;//not used!
	CLockCriticalSection      m_csList;
};

//自由观察者定义类
class CFreeObserver : public CObserver  
{
public:
	CFreeObserver();
	virtual ~CFreeObserver();
    
	virtual BOOL Update(IMsgData *pInfo, UINT uInfoID, WPARAM wPara,LPARAM lPara,int nMsgLen);    
protected:
	CLockCriticalSection m_csUpdate;
};

//被观察者定义,
//可以进行一对多消息通知。
class CObservedObj  
{
public:
	CObservedObj();
	virtual ~CObservedObj();
	
	virtual void Attatch(CObserver *pObserver);
	virtual void Detatch(CObserver *pObserver);
protected:
	//这里的ImsgData类型和应用架构绑定的有点儿紧。
	virtual BOOL Notify(IMsgData *pMsg,int nMsgType,WPARAM wParam,LPARAM lParam,int nLen);    
    
protected:
	list<CObserver *> m_listObserver;
	CLockCriticalSection m_csObservers;
};

typedef CObservedObj   IObservedObj;

//处理管道定义
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

#define   __EE       0.00669342162296594323
#define   __A        6378245.0
