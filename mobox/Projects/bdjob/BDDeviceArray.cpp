#include "BDDeviceArray.h"
//#include "../BDProtocal/protocal/ChnavtectGlobal.h"
#include "ChnavtectGlobal.h"
#include "Instance.h"
//#include "../BDProtocal/ProtoInterface.h"
#include "ProtoInterface.h"
#include <algorithm>
using namespace std;

CBDDeviceArray * CBDDeviceArray::m_pArray = NULL;

CLockCriticalSection CBDDeviceArray::m_lock;

CBDDeviceArray * CBDDeviceArray::Get()
{
	AUTOLOCK(m_lock);
	if(m_pArray == NULL)
		m_pArray = new CBDDeviceArray();

	return m_pArray;
}

CBDDeviceArray::CBDDeviceArray(void)
{
	m_thHandle = NULL;
	m_cb = NULL;
}


CBDDeviceArray::~CBDDeviceArray(void)
{
}

void CBDDeviceArray::InitThread()//时间检测
{
	DWORD dwId;
	m_thHandle = CreateThread(0,0,_timercheck,this,0,&dwId);
}

void CBDDeviceArray::SetDeviceStatus(CDevice* pDevice , BOOL bSendOk,int waitseconds)
{
	AUTOLOCK(m_lock);

	if(bSendOk){
		if (m_mapDevWork[pDevice].lastsendlen > 0 && m_mapDevWork[pDevice].datalen >= m_mapDevWork[pDevice].lastsendlen){
			m_mapDevWork[pDevice].datalen = m_mapDevWork[pDevice].datalen - m_mapDevWork[pDevice].lastsendlen;
			if(m_mapDevWork[pDevice].datalen>0)
				memmove(m_mapDevWork[pDevice].payload,m_mapDevWork[pDevice].payload+m_mapDevWork[pDevice].lastsendlen,m_mapDevWork[pDevice].datalen);
		}
		m_mapDevTime[pDevice] = pDevice->GetInStationFreq();
	}else{
		m_mapDevWork[pDevice].lastsendlen = 0;
		m_mapDevTime[pDevice] = waitseconds;		
	}		
}
void CBDDeviceArray::InsertWorkings(CDevice* pDevice)
{
	m_workings.push_back(pDevice);
}

void CBDDeviceArray::AssignWork(CDevice* pDevice, char* databuffer, int datalen, int dest)
{
	AUTOLOCK(m_lock);

	__assureworkdata(&m_mapDevWork[pDevice],databuffer,datalen);
	m_mapDevWork[pDevice].dest = dest;
	m_mapDevWork[pDevice].ackfail_times = 0;
	m_workings.push_back(pDevice);
}

BOOL  CBDDeviceArray::HasFreeDevice()
{
	AUTOLOCK(m_lock);

	if (m_idles.size() == 0) return FALSE;

	CDevice * dev = m_idles.front();
	m_determinings.push_back(dev);
	m_idles.pop_front();

	m_mapDetermineTime[dev] = time(0);

	return TRUE;

}

const CDevice* CBDDeviceArray::GetFreeDevice()
{
	AUTOLOCK(m_lock);

	if (m_determinings.size() == 0) HasFreeDevice();

	if (m_determinings.size() == 0) return NULL;

	const CDevice* dev = m_determinings.front();
	m_determinings.pop_front();

	return dev;
}

BOOL CBDDeviceArray::IsDeviceFree(CDevice* pdev)
{
	int ticks = m_mapDevTime[pdev];
	int datalen = m_mapDevWork[pdev].datalen;

	if(ticks <=0 && datalen == 0 /*&& pdev->IsPending()==FALSE*/) 
		return TRUE;
	else 
		return FALSE;
}
void CBDDeviceArray::ChechDeviceState()
{
	list<CDevice*>::iterator it = m_workings.begin();
	while (it != m_workings.end())
	{
		CDevice* p = *it;
		if (p->IsNormal() == FALSE)
		{
			it = m_workings.erase(it);
			continue;
		}

		//设备断电重启后，有可能不上报BSI信息，因此增加此处理过程。
		if (p->IsOffline()) p->InitDevice();//2018-08-15
		

		it++;
	}
}

void CBDDeviceArray::CheckFreeList()
{
	list<CDevice*>::iterator it = m_workings.begin();
	while(it != m_workings.end())
	{
		CDevice* p = *it;

		//设备断电重启后，有可能不上报BSI信息，因此增加此处理过程。
		if ((*it)->IsOffline()) (*it)->InitDevice();//2018-08-15

		if(p->IsInit()){
			m_mapDevTime[p]--;
			if(IsDeviceFree(p)){
				it = m_workings.erase(it);
				m_idles.push_back(p);
				continue;
			}
			else if (p->IsPending()){//工作状态中的一个分状态。
				if(m_mapDevTime[p] < 0){
					p->SetPending(FALSE);
					m_mapDevWork[p].datalen = 0;
					m_mapDevTime[p] = 0;
				}
			}
		}
		
		it++;
	}	
}

void CBDDeviceArray::CheckDetermineList()
{
	list<CDevice*>::iterator it = m_determinings.begin();
	while (it != m_determinings.end())
	{
		if (time(0) - m_mapDetermineTime[*it] > 3){
			m_idles.push_back(*it);
			it = m_determinings.erase(it);
			continue;
		}

		//设备断电重启后，有可能不上报BSI信息，因此增加此处理过程。
		if ((*it)->IsOffline()) (*it)->InitDevice();//2018-08-15

		it++;
	}
}

int CBDDeviceArray::SendComm(CDevice* p)
{
	TXAInfo txa;
	memset(&txa,0,sizeof(TXAInfo));

	int combits = p->GetCommBitsLen(FALSE); //最大通信长度
	if (combits <= 8) return 0;
	int bytes = (combits-8)/8;

	txa.byCodeType = 2;
	txa.byCommCat = 1;
	txa.ifAnswer = FALSE;
	txa.ifcmd = 0;
	txa.nLocalId = p->GetLocalId();
	txa.nUserId = m_mapDevWork[p].dest;

	if(bytes > m_mapDevWork[p].datalen)
		bytes = m_mapDevWork[p].datalen;

	memcpy(txa.chContent,m_mapDevWork[p].payload,bytes);
	txa.nContentLen = bytes;//strlen(txa.chContent);
	BOOL bOk = p->command(DEVICE_BDTXA, &txa, txa.nContentLen * 8); //发送通信

	if(bOk){
		printf("-------------------------device %d send %d bytes ok.\n",p->GetLocalId(),bytes);
		p->SetPending(TRUE);
	}else{
		printf("-------------------------device %d send %d bytes failed.\n",p->GetLocalId(),bytes);
	}	

	if(bOk) 
		return bytes;
	else 
		return 0;
}

void CBDDeviceArray::DoWork()
{
	if(m_workings.size()==0) return;

	list<CDevice*>::iterator it = m_workings.begin();
	
	static time_t last = 0;
	if (last == 0) last = time(NULL);

	time_t now = time(NULL);
	if (now - last > 15){
		last = now;

	}
//	printf("working list ----------\n");

	while(it != m_workings.end())
	{
		CDevice* p = *it;
		if(p->IsInit()){
			int ticks = m_mapDevTime[p];

			printf("device id is %d life :%d is pending:%d\n",p->GetLocalId(),ticks,p->IsPending());
			if(ticks <=0 && m_mapDevWork[p].datalen>0 && p->IsPending()== FALSE){				
				int bytes = SendComm(p);
				m_mapDevWork[p].datalen = 0;
				m_mapDevWork[p].lastsendlen = bytes;	
				m_mapDevTime[p] = p->GetInStationFreq();
			}
		}else
			p->InitDevice();
		
		it++;
	}	
}

DWORD WINAPI CBDDeviceArray:: _timercheck(void* pvoid)
{
	CBDDeviceArray* pArray = (CBDDeviceArray*)pvoid;

	if(pArray == NULL) return -1;

	while(true)
	{
		m_lock.Lock();

		pArray->ChechDeviceState();//检查设备是否能正常工作
		pArray->CheckDetermineList();
		pArray->CheckFreeList();

		pArray->DoWork();

		m_lock.Unlock();

		Sleep(1000);
	}
	

}