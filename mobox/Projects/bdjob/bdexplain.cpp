#include <stdlib.h>
#include <string.h>
#include "bdexplain.h"
#include "include/ConstDefine.h"
#include "include/Head.h"
#include "include/ProtoInterface.h"
#include "Instance.h"
#include "RemoteServce.h"
#include "global.h"
#include "p2pclient.h"
#include "tool.h"
//通信长度


static int   g_CommCapacity[2][4] = { { 140, 360, 580, 1680 }, { 110, 408, 628, 848 } }; //保密用户,非密用户
int  HexToAsc(char *destbuff, unsigned int destbufflen, unsigned char *srchexbuff, int srcbitscnt)
{
	char chdata;
	int i = 0;

	for (; i < srcbitscnt;)
	{
		if (i % 8 == 0){
			__hex_char(srchexbuff[i / 8] >> 4, chdata);
		}
		else{
			__hex_char(srchexbuff[i / 8] & 0x0F, chdata);
		}

		destbuff[i / 4] = chdata;
		i += 4;
	}
	return i / 4;
}
int bytes_hexchars(char DestBuffer[], int nDestBufferLen, BYTE byBitArray[], int nBitsNum)
{
	return HexToAsc(DestBuffer, nDestBufferLen, byBitArray, nBitsNum);
}

CDevice::CDevice(){
	InitializeCriticalSection(&m_cs);

	InitSelf();
}

CDevice::~CDevice(){ 
	ClearSelf();

	DeleteCriticalSection(&m_cs);
}

void CDevice::InitSelf()
{
	m_connection = NULL;
	m_messager = NULL;
	m_searchexp = NULL;
	m_cb = NULL;
	m_bOpen = FALSE;
	m_bInit = FALSE;
	m_bStop = TRUE;
	m_bPending = FALSE;
	m_icInfo = NULL;
	m_thread = NULL;
	m_bNormal = TRUE;
	m_thid = 0;
	
	
}
void CDevice::ClearSelf()
{
	if (m_bOpen == TRUE){
		if (m_connection)
		{
			m_connection->Release();
			m_connection = NULL;
		}

		if (m_searchexp){
			m_searchexp->Release();
			m_searchexp = NULL;
		}
		
	}
	delete m_icInfo;	

	InitSelf();
}
BOOL CDevice::IsOpened()
{
	return m_bOpen;
}

//bComSuccess 串口连接是否成功
BOOL CDevice::Open(protocalname_t pProtocalType, const int nPort, const int nBaudRate, const int nDataBits, const int nStopBits, BOOL bIfParity, const int nParity, const char* desc, pTFunMsgCallback callback, bool& bComSuccess)
{
	if (m_connection == NULL)
	{
		m_bOpen = TRUE;

		BOOL bOpen = open_com_connection(nPort, nBaudRate, nDataBits, nStopBits, bIfParity, nParity, &m_connection);
		if (bOpen && m_connection){
			bComSuccess = true;
			if (m_searchexp == NULL){
				m_searchexp = prorotocal_init_cpp(pProtocalType, m_messager); //协议类型
				if (m_searchexp)
					m_messager->Advise(this);
				else
					m_bOpen = FALSE;
			}

			if (m_bOpen){
				m_cb = callback;
				m_bStop = FALSE;
				m_thread = ::CreateThread(NULL, 0, CDevice::worker, this, 0, &m_thid);//工作线程
				if (!m_thread)
					m_bOpen = FALSE;
			}

		}
		else{
			bComSuccess = false;
			m_bOpen = FALSE;
		}

		if (m_bOpen == FALSE){
			ClearSelf();
			return FALSE;
		}
		else
			return m_bOpen;

	}
	else
		return -1;
}

//发送IC检测
void  CDevice::InitDevice()
{
	ICAInfo ica;
	ica.byType = 0;
	ica.nLocalID = 0;
	ica.nSubUsrFrameNo = 0;
	int iMsgBlockLen = sizeof(ICAInfo);
	command(DEVICE_BDICA, &ica, iMsgBlockLen);	//发送ic检测
	// lq modified
	if (m_icInfo  == NULL )  m_icInfo = new ICIInfo;
}
//发送波束检测信息
void   CDevice::SendBSI()
{
	RMOInfo rmo;
	rmo.byMode = 2;
	strcpy(rmo.chName, "BSI");
	rmo.nOutFreq = 1;
	command(DEVICE_BDRMO, &rmo, sizeof(RMOInfo));
}

void   CDevice::SendZTI()
{
	RMOInfo rmo;
	rmo.byMode = 2;
	strcpy(rmo.chName, "ZTI");
	rmo.nOutFreq = 1;
	command(DEVICE_BDRMO, &rmo, sizeof(RMOInfo));
}
void CDevice::Close()
{
	Update(CUNPACK_CLOSE, NULL, 0);
	ClearSelf();
}
DWORD WINAPI CDevice::worker(void* pvoid)
{
	CDevice * device = (CDevice*)pvoid;

	if (device == NULL) return 0;
	if (device->m_connection == NULL) return 0;
	if (device->m_searchexp == NULL)  return 0;
	
	BYTE byBuffer[1024];
	int  nReadLen = 0;

	while (!device->m_bStop)
	{
		//test code
		//char *strtext = "$BDTXR,1,0195817,2,,A4FB113383315933313233343536*37\r\n";
		//char *strtext = "$BDTXR,1,0139311,2,,A4FB00003535383736306566*34\r\n";
		//char *strtext = "$BDTXR,1,0139312,2,,A4FB200018667864646473C0B4C8E2CAD6CBB5C7D0C8C3*3B\r\n";
		///char *strtext = "$BDWAA,0,,0312194,072236.37,3802.69000,N,11426.16000,E,61,M*05\r\n";
		//char *strtext = "$BDWAA,0,,0312194,072236.40,3802.69000,N,11526.16000,E,61,M*04\r\n";
		//char *strtext = "$BDWAA,0,,0241638,072236.37,3802.69000,N,11426.16000,E,61,M*03\r\n";
		//char *strtext = "$BDTXR,1,195817,2,1439,A4BF0113ECCB13ECCBHELLO*49\r\n";
		//char *strtext = "$BDTXR,1,195817,2,1439,A4BF0113ECCB13ECCB48656C6C6F*74\r\n";
		//nReadLen = strlen(strtext);
		//strcpy((char*)byBuffer, strtext);
		//device->m_searchexp->ProcessData(byBuffer, nReadLen);
		//Sleep(1);
		
		nReadLen = 0;
		BOOL bOk = device->m_connection->ReadData(byBuffer, sizeof(byBuffer), nReadLen);
		if (!bOk){
			Sleep(50);//休眠
		}
		else{			
			    //nReadLen = strlen(strtext);
			    //strcpy((char*)byBuffer, strtext);
			    device->m_searchexp->ProcessData(byBuffer, nReadLen);//搜索

			}
			Sleep(1);//休眠
		//Sleep(1000000);
	}
	
	return TRUE;
}
BOOL  CDevice::command(int msgid, void* msg, int msgblocklen)
{
	BOOL rtn = FALSE;
	BYTE buffer[1024];

	if (msg == NULL || msgblocklen <= 0) return FALSE;
	if (m_searchexp == NULL || m_connection == NULL) return FALSE;

	if (m_searchexp == NULL) return FALSE;
	if (msgid == DEVICE_BDTXA){
		int i = 0; 
		i++;
	}

	Sleep(0);

	EnterCriticalSection(&m_cs);
	int len = m_searchexp->EncodeFrame(msg, msgid, msgblocklen, buffer, sizeof(buffer));
	if (len){
		int writed = m_connection->WriteData(buffer, len);
		buffer[writed] = '\0';
		//printf((char*)buffer);
		//printf("\n");
		if (writed == len)
		{
			rtn = TRUE;
			if (writed > 30)
			{
				printf("WriteData[len = %d]: %s\n", writed, buffer);
			}
		}
		if (writed == 0){
			Close();
		}
	}

	LeaveCriticalSection(&m_cs);

	return rtn;
}
int  CDevice::GetInStationFreq()//得到服务频度
{
	if (m_icInfo == NULL)
	{
		printf("mcic == NULL\r\n");
	}
	return m_icInfo->nServiceFreq;
}
int CDevice::GetLocalId()
{
	if (m_icInfo != NULL)
	  return m_icInfo->nLocalId;
	else
	  return 0;
	
}

BOOL CDevice::IsOffline()
{
	time_t now = time(0);

	if (now - m_activetime > 30){
		return TRUE;
	}

	return FALSE;
}

BOOL CDevice::IsNormal()
{
	return m_bNormal;
}
BOOL CDevice::IsInit()
{
	return m_bInit;
}

void CDevice::Update(const UINT uMsgId, const void * pMsgBuffer, int para)
{
	m_activetime = time(0);

	switch (uMsgId)
	{
	case CUNPACK_ICI: //接收IC检测信息
	{
		lpICIInfo pICIInfo = (lpICIInfo)pMsgBuffer;
		memcpy(m_icInfo, pICIInfo, sizeof(ICIInfo));
		m_bInit = TRUE; //初始化
		//log_write(g_instapp->log, LOG_NOTICE, "device %d init ok, instation freq is %ds..", m_icInfo->nLocalId, m_icInfo->nServiceFreq);
		//printf("device %d init ok, instation freq is %ds..\n", m_icInfo->nLocalId, m_icInfo->nServiceFreq);
		//SendZTI();//2018-08-15
		SendBSI();
	}
	break;
	case CUNPACK_BSI: //功率检测回复
	{

		lpBeamInfo  beamInfo = (lpBeamInfo)pMsgBuffer;
		BYTE bySignal[10];
		for (int i = 0; i < 10; i++){
			bySignal[i] = beamInfo->byPower[i];
		}
		log_write(g_instapp->log, LOG_NOTICE, "%d:BSI[%d %d %d %d %d %d %d %d %d %d].", m_icInfo->nLocalId, bySignal[0], bySignal[1], bySignal[2], bySignal[3], bySignal[4], bySignal[5], bySignal[6], bySignal[7], bySignal[8], bySignal[9]);

		//发给beidouserver
		char buffer[1024] = { 0 };
		int printn = 0;
		printn += sprintf_s(buffer + printn, sizeof(buffer) - printn, "$BDBSI,%d,([%d,%d,%d,%d,%d,%d,%d,%d,%d,%d])A4", m_icInfo->nLocalId, bySignal[0], bySignal[1], bySignal[2], bySignal[3], bySignal[4], bySignal[5], bySignal[6], bySignal[7], bySignal[8], bySignal[9]);
		printn += sprintf_s(buffer + printn, sizeof(buffer) - printn, "*%02X\r\n", checksum((unsigned char*)buffer, printn));
		response_pkt_p pkt;
		pkt = response_pkt_new(g_instapp->pktpool, strlen((const char*)buffer));
		memcpy(pkt->data, buffer, strlen((const char*)buffer));
		pkt->len = strlen((const char*)buffer);
		pkt->data[pkt->len] = '\0';
		p2pclient_write((p2pclient_t)g_instapp->p2pbdsvr, pkt);//打包发送给bdsvr

	}
	break;
	case CUNPACK_ZTI:
	{
		BOOL byizhi = FALSE;
		BOOL bjingmo = FALSE;
		lpZTIInfo pZTIinfo = (lpZTIInfo)pMsgBuffer;
		BYTE icstate = pZTIinfo->data.selfcheck.ICCardState;
		BYTE ihardstate = pZTIinfo->data.selfcheck.HardState;
		BYTE iinstate = pZTIinfo->data.selfcheck.InState;
		if (ihardstate & 0x01 == 1) //天线未连接
			m_bNormal = FALSE;
		else if (iinstate & 0x01 == 1) //抑制状态
		{
			m_bNormal = FALSE;
			byizhi = TRUE;
		}
		else if (iinstate & 0x02 != 1)
		{
			m_bNormal = FALSE;//静默状态
			bjingmo = TRUE;
		}
		else
			m_bNormal = TRUE;
		//log_write(g_instapp->log, LOG_NOTICE, "ZTI[抑制状态:%d  静默状态:%d ].", byizhi, bjingmo);
	}
	break;
	case CUNPACK_TXR: //接收通信信息(可以监听下属用户的通信信息)
	{
		lpTXRInfo pTXRInfo = (lpTXRInfo)pMsgBuffer;
		printf("pTXRInfo->chContent = %s\r\n", (unsigned char*)pTXRInfo->chContent);
		{
			char strtime[64] = { 0 };
			char buffer[1024] = { 0 };
			SYSTEMTIME t;
			GetLocalTime(&t);
			sprintf_s(strtime, sizeof(strtime), "%02d%02d", t.wHour, t.wMinute);
			//发给beidouserver
			int printn = 0;
			printn += sprintf_s(buffer + printn, sizeof(buffer) - printn, "$BDTXR,%d,%d,%d,%s,A4", pTXRInfo->byInfoType, pTXRInfo->nLocalID, 2/*混合传输*/, strtime);
			int bytes = bytes_hexchars(buffer + printn, sizeof(buffer) - printn, (unsigned char*)pTXRInfo->chContent, pTXRInfo->nContentLen * 8);
			printn += bytes;
			printn += sprintf_s(buffer + printn, sizeof(buffer) - printn, "*%02X\r\n", checksum((unsigned char*)buffer, printn));
			response_pkt_p pkt;
			pkt = response_pkt_new(g_instapp->pktpool, strlen((const char*)buffer));
			memcpy(pkt->data, buffer, strlen((const char*)buffer));
			pkt->len = strlen((const char*)buffer);
			pkt->data[pkt->len] = '\0';
			p2pclient_write((p2pclient_t)g_instapp->p2pbdsvr, pkt);//打包发送给bdsvr
			log_write(g_instapp->log, LOG_NOTICE, "send --TXR $BDTXR-- to bdsvr:%s", buffer);

		}

	}
	break;
	case CUNPACK_FKI:
	{
		m_bPending = FALSE;
		lpFKIInfo pFKIInfo = (lpFKIInfo)pMsgBuffer;
		if (pFKIInfo->chExeRslt == 'Y')
		{
			//g_Remoteinstance.GetBDArray()->SetDeviceStatus(this, TRUE, GetInStationFreq());
			printf("------------------------%d fki indicate ok.. wait %ds\n", GetLocalId(), GetInStationFreq());
		}
		else
		{
			if (pFKIInfo->byTranInhabitInd){ //发射被抑制
				//g_Remoteinstance.GetBDArray()->SetDeviceStatus(this, FALSE, GetInStationFreq());
			}
			else{
				if (g_instapp->ihascache == 1)
				{
					//g_Remoteinstance.GetBDArray()->SetDeviceStatus(this, TRUE, GetInStationFreq());
					printf("%d fki indicate ok.. wait %ds\n", GetLocalId(), GetInStationFreq());
				}
				else
				{
					//g_Remoteinstance.GetBDArray()->SetDeviceStatus(this, FALSE, pFKIInfo->byMinuteWait * 60 + pFKIInfo->bySecondWait);
					printf("%d fki indicate failed.. wait %ds\n", GetLocalId(), GetInStationFreq());
				}

			}

		}

	}
	break;
	case CUNPACK_DWR: //位置信息
	{
		char buffer[1024] = { 0 };
		int printn = 0;
		lpDWRInfo pDWRInfo = (lpDWRInfo)pMsgBuffer;
		double dlat = pDWRInfo->dfLat;
		double dlong = pDWRInfo->dfLong;

		int isecond = (int)pDWRInfo->fPosSecond;
		int iminsecond = (int)((pDWRInfo->fPosSecond - isecond) * 60);
		int  latdegree = (int)dlat;
		int  latsec = (dlat - latdegree) * 60;
		int ilatmiao = (int)(((dlat - latdegree) * 60 - latsec) * 60);
		int iflongdu = (int)dlong;
		int iflongfen = (int)((dlong - iflongdu) * 60);
		int ilongmiao = (int)(((dlong - iflongdu) * 60 - iflongfen) * 60);
		char cUrgent = 'V';
		char cMultiValue = 'V';
		char cGaoKong = 'L';
		if (pDWRInfo->bUrgent == true)
		{
			cUrgent = 'A';
		}
		if (pDWRInfo->bMultiValue == true)
		{
			cMultiValue = 'A';
		}
		if (pDWRInfo->bGaoKong == true)
		{
			cGaoKong = 'H';
		}
		printn += sprintf_s(buffer + printn, sizeof(buffer) - printn, "$DWR,%d,%07d,%02d%02d%02d.%02d,%02d%02d.%02d,%c,%03d%02d.%02d,%c,%.1f,%c,%.1f,%c,%d,%c,%c,%c", pDWRInfo->nPosType, pDWRInfo->nLocalId, pDWRInfo->nPosHour,
			pDWRInfo->nPosMinute, isecond, iminsecond, latdegree, latsec, ilatmiao, pDWRInfo->chLatDirection, iflongdu, iflongfen, ilongmiao,
			pDWRInfo->chLongDirection, pDWRInfo->fEarthHgt, pDWRInfo->chHgtUnit, pDWRInfo->dfHgtOut, pDWRInfo->chHgtOutUnit, pDWRInfo->byPrecisionInd,
			cUrgent, cMultiValue, cGaoKong);
		printn += sprintf_s(buffer + printn, sizeof(buffer) - printn, "*%02X\r\n", checksum((unsigned char*)buffer, printn));

		//发给beidouserver
		response_pkt_p pkt;
		pkt = response_pkt_new(g_instapp->pktpool, strlen((const char*)buffer));
		memcpy(pkt->data, buffer, strlen((const char*)buffer));
		pkt->len = strlen((const char*)buffer);
		p2pclient_write((p2pclient_t)g_instapp->p2pbdsvr, pkt);//打包发送给bdsvr
		log_write(g_instapp->log, LOG_NOTICE, "send --DWR $DWR-- to bdsvr.");
	}
	break;
	case CUNPACK_WAA: //位置报告1  
	{
		char buffer[1024] = { 0 };
		int printn = 0;
		lpWAAInfo pWAAInfo = (lpWAAInfo)pMsgBuffer;
		lpDWRInfo pDWRInfo = new DWRInfo;
		pDWRInfo->nPosType = 3;
		pDWRInfo->nLocalId = pWAAInfo->nUserId;
		pDWRInfo->dfLat = pWAAInfo->dfLat;
		pDWRInfo->dfLong = pWAAInfo->dfLong;
		pDWRInfo->nPosHour = pWAAInfo->wReportHour;
		pDWRInfo->nPosMinute = pWAAInfo->wReportMinute;
		pDWRInfo->fPosSecond = pWAAInfo->fReportSecond;
		pDWRInfo->bUrgent = false;
		pDWRInfo->bMultiValue = false;
		pDWRInfo->bGaoKong = false;
		pDWRInfo->fEarthHgt = pWAAInfo->dfHgt;
		double dlat = pDWRInfo->dfLat;
		double dlong = pDWRInfo->dfLong;
		int isecond = (int)pDWRInfo->fPosSecond;
		int iminsecond = (int)((pDWRInfo->fPosSecond - isecond) * 60);

		int latdegree = (int)dlat;
		double latmin = (dlat - latdegree) * 60;

		int iflongdu = (int)dlong;
		double iflongfen = ((dlong - iflongdu) * 60);

		char cUrgent = 'V';
		char cMultiValue = 'V';
		char cGaoKong = 'L';
		if (pDWRInfo->bUrgent == true)
		{
			cUrgent = 'A';
		}
		if (pDWRInfo->bMultiValue == true)
		{
			cMultiValue = 'A';
		}
		if (pDWRInfo->bGaoKong == true)
		{
			cGaoKong = 'H';
		}

		printn += sprintf_s(buffer + printn, sizeof(buffer) - printn, "$DWR,%d,%07d,%02d%02d%02d.%d,%02d%.3f,%c,%03d%.3f,%c,%.1f,%c,%.1f,%c,%d,%c,%c,%c", pDWRInfo->nPosType, pDWRInfo->nLocalId, pDWRInfo->nPosHour,
			pDWRInfo->nPosMinute, isecond, iminsecond, latdegree, latmin, pDWRInfo->chLatDirection, iflongdu, iflongfen,
			pDWRInfo->chLongDirection, pDWRInfo->fEarthHgt, pDWRInfo->chHgtUnit, pDWRInfo->dfHgtOut, pDWRInfo->chHgtOutUnit, pDWRInfo->byPrecisionInd,
			cUrgent, cMultiValue, cGaoKong);
		printn += sprintf_s(buffer + printn, sizeof(buffer) - printn, "*%02X\r\n", checksum((unsigned char*)buffer, printn));

		//发给beidouserver
		response_pkt_p pkt;
		pkt = response_pkt_new(g_instapp->pktpool, strlen((const char*)buffer));
		memcpy(pkt->data, buffer, strlen((const char*)buffer));
		pkt->len = strlen((const char*)buffer);
		p2pclient_write((p2pclient_t)g_instapp->p2pbdsvr, pkt);//打包发送给bdsvr
		log_write(g_instapp->log, LOG_NOTICE, "send --WAA $DWR-- to bdsvr.");
		delete pDWRInfo;
	}
	break;

	default:
	{

	}
	break;
	}
}
//
//bUrgent = 0是普通通信 , bUrgent= 1是特快通信
int CDevice::GetCommBitsLen(BOOL bUrgent)
{
	int nLen = 0;
	if (!bUrgent) //普通通信
	{
		if (m_icInfo->byCommLevel > 0 && m_icInfo->byCommLevel < 5)
		{
			if (m_icInfo->chEncriptFlag == 'E') //加密用户
			{
				nLen = g_CommCapacity[0][m_icInfo->byCommLevel - 1];
			}
			else //非加密
				nLen = g_CommCapacity[1][m_icInfo->byCommLevel - 1];
		}
	}
	else //特快通信
		nLen = 188;

	return nLen;
}
