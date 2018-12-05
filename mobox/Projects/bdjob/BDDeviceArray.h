#pragma once
#include <vector>
#include <list>
#include <map>
#include "bdexplain.h"
using namespace  std;

typedef struct _workdata_st{
	_workdata_st(){
	   datalen = 0;
	   maxlen = 512;
	   payload = (char*)calloc(1,512);
	   ackfail_times = 0;
	}
	char *payload;
	int   maxlen;
	int  datalen;
	int  lastsendlen;
	int  dest;
	int  ackfail_times;
	int  live_seconds;
}workdata_st,*workdata_t;

#define __workdatalen(_workdata_) (_workdata_.datalen)
#define __assureworkdata(_workdata_,_databuffer_,_datalen_)\
{\
	if(_datalen_>(_workdata_)->maxlen)\
	{\
		realloc((_workdata_)->payload,_datalen_);\
		(_workdata_)->maxlen = _datalen_;\
	}\
	memcpy((_workdata_)->payload,_databuffer_,_datalen_);\
	(_workdata_)->datalen=_datalen_;\
}

typedef int  (*pfn_bdmsgcallback)(int msgid,const void* msg,unsigned int param,int xtra);
#include "bdexplain.h"

class CBDDeviceArray
{
public:
	static CBDDeviceArray * Get();
	~CBDDeviceArray(void);

	void InitThread();
	
	void SetDeviceStatus(CDevice* pDevice, BOOL bSendOk, int waitseconds);

	const CDevice* GetFreeDevice();
	BOOL  HasFreeDevice();

	void AssignWork(CDevice* pDevice/*free device*/, char* databuffer, int datalen, int dest);
	void SetCallback(pfn_bdmsgcallback cb){m_cb = cb;}
	void InsertWorkings(CDevice* pDevice);
protected:
	CBDDeviceArray(void);
	static CBDDeviceArray * m_pArray;
	static DWORD WINAPI _timercheck(void* pvoid);
	static CLockCriticalSection m_lock;	
	void CheckFreeList();
	void CheckDetermineList();
	void ChechDeviceState();
	void DoWork();
	BOOL IsDeviceFree(CDevice* pdev);
	int  SendComm(CDevice* pdev);
private:
	map<CDevice*, int> m_mapDevTime;
	map<CDevice*, int> m_mapDetermineTime;
	map<CDevice*, workdata_st> m_mapDevWork;
	list<CDevice*>  m_idles;
	list<CDevice*>  m_determinings;
	list<CDevice*>  m_workings;
	
	HANDLE m_thHandle;
	pfn_bdmsgcallback m_cb;
};

