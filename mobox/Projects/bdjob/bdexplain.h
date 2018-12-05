//北斗收数据
#ifndef  _BD_EXPLAIN_H_
#define  _BD_EXPLAIN_H_

//#include "../BDProtocal/protocal/ChnavtectGlobal.h"
#include "ChnavtectGlobal.h"
#include "include/Head.h"
#include "include/ConstDefine.h"


#define __hex_char(_hexdata_,_chardata_) \
{   if((_hexdata_)<10&&(_hexdata_)>=0) \
		_chardata_ = (_hexdata_) + '0';\
				else if((_hexdata_)>=10&&(_hexdata_) <16)\
		_chardata_ = (_hexdata_) - 10 + 'A';\
				else _chardata_ ='0';\
}
typedef struct tagICIInfo
{
	int nLocalId;
	int nSerialNo;      //序列号
	int nBroadId;     //对于普通用户机,为接收通播ID;对具备指挥功能的用户设备,为发送通播ID。
	int nUsrAttribute; //用户特征值:0:智慧型用户机1:一类用户机2:二类用户机3:三类用户机4:指挥型用户机(进行身份认证)5:一类用户机(进行身份认证)6:二类用户机(进行身份认证)7:三类用户机(进行身份认证)
	int  nServiceFreq; //单位:秒
	BYTE byCommLevel;  //通信等级:1-4
	char chEncriptFlag;//加密标识。'E':加密,'N':非密。
	int  nSubUsersNum; //下属用户。
}ICIInfo, *lpICIInfo;

typedef struct tagICAInfo
{
	BYTE byType;//0:检测本机加解密模块信息。1:检测具备指挥功能用户设备的下属用户信息。
	int  nLocalID;
	int  nSubUsrFrameNo;//当指令类型等于0时，下属用户信息帧号填0.
}ICAInfo, *pICAInfo;

typedef struct tagICZInfo
{
	int nTotalSubUserNums; //总下属用户数
	int nUsrCntInFrm;
	int nSubUserIds[100];
}ICZInfo, *lpICZInfo;

//通信信息常量定义 : CUNPACK_TXR 
typedef struct tagTXRInfo
{
	BYTE byInfoType;//信息类别:1~5。1:普通通信，2:特快通信，3:通播通信,4:按最新存入电文查询获得的通信,5:按发信方地址查询获得的通信。
	int  nLocalID;//发信方地址
	int  nDestionID; //收信方地址
	BYTE byTeleFormat;//电文形式:0-汉字，1-代码，2-混合传输
	BYTE bySendHour;  //发信时间(时)。byInfoType == 1，2，3时，发信时间无效。byInfoType ==4，5时,为被查询的通信电文在中心控制系统记录的发送时间。
	BYTE bySendMinute;//发信时间(分)。
	char chContent[1024];//通信内容。C串。当byTeleFormat == 2时，电文内容首字母固定为A4.
	int  nContentLen;
	BYTE byCRC; //CRC校验，0-正确 1-错误
}TXRInfo, *lpTXRInfo;


typedef struct tagFKIInfo
{
	char chCommandName[5];//指令名称:如"DWT"
	char chExeRslt;//执行情况:'Y':执行成功,'N':执行失败。
	char chIntervalInd;//'Y':频度设置无误,'N':频度设置错误.
	BYTE byTranInhabitInd;//发射抑制指示.0:发射抑制解除，1:接收到系统的抑制指令，发射被抑制，2:由于电量不足，发射被抑制,3:由于无线电静默,发射被抑制。
	BYTE byMinuteWait;    //指令需要等待的时间,分部分
	BYTE bySecondWait;    //指令需要等待的时间,秒部分
}FKIInfo, *lpFKIInfo;


//定位信息
//定位信息, 消息定义常量 CUNPACK_DWR 
typedef struct tagDWRInfo
{
	int    nLocalId;//nPosType==1:本终端的ID,==2:被查询用户的用户地址,==3:发送位置报告方的用户地址。
	int    nPosType;//定位信息类型。1:本终端定位申请返回的定位信息,2:定位查询返回的下属子用户位置信息，3:接收到的位置报告信息。
	int    nPosHour;
	int    nPosMinute;
	float  fPosSecond;
	double dfLat;
	char   chLatDirection;//纬度方向:'N'/'S';
	double dfLong;
	char   chLongDirection;//经度方向:'E'/'W';
	float  fEarthHgt;      //大地高.
	char   chHgtUnit;      //大地高单位。
	double dfHgtOut;       //高程异常
	char   chHgtOutUnit;   //高程异常单位
	BYTE   byPrecisionInd; //定位精度指示.0:一档20米精度;1:二档100米精度
	BOOL   bUrgent;        //是否紧急
	BOOL   bMultiValue;    //是否多值解
	BOOL   bGaoKong;       //高空指示

}DWRInfo, *lpDWRInfo;

//位置报告1双向语句 ：DEVICE_WAA
typedef struct tagWAAInfo
{
	int    nDestionID; //接收方ID（新增）GJ 2016.3.29
	BYTE   byInfoType;//信息类型。0-表示为用户设备接收的位置报告信息，此时nReportFreq无效。nUserId为发送位置报告的用户的地址。
	//1-表示为用户设备发送的位置报告信息，此时nUserId为接收位置报告信息的用户地址。
	int    nReportFreq; //报告频度，单位:秒。
	int    nUserId;  //发信方ID
	WORD   wReportHour;//位置报告时间信息
	WORD   wReportMinute;
	float  fReportSecond;
	double dfLat;//纬度，单位:度。
	char   chLatDir;//'N'/'S';
	double dfLong;//经度，单位:度。
	char   chLongDir;//'E'/'W';
	double dfHgt;//高程
	char   chHgtUnit;//高程单位:m
}WAAInfo, *lpWAAInfo;

//数据输出控制指令 DEVICE_RMO
typedef struct tagRMOInfo
{
	char chName[4];//合法的标识串，如"GGA"
	BYTE byMode;   //模式。1:关闭指定语句，2:打开指定语句，3:关闭全部语句，4:打开全部语句。byMode == 3或byMode ==4时:目标语句数据保留区为空。
	int  nOutFreq; //单位:秒。byMode == 4时,此保留区域为空。
}RMOInfo, *lpRMOInfo;

//自检信息
//CUNPACK_ZJXX
typedef struct tagSelfCheckInfo
{
	DWORD LocalID;
	BYTE ICCardState;		// IC卡状态
	BYTE HardState;		// 硬件状态
	BYTE Batterystate;		// 电池电量
	BYTE InState;			// 入站状态
	BYTE PowerState[10];	// 功率状况
}SelfCheckInfo, *lpSelfCheckInfo;

struct zti_st{
	BYTE   byStatusMask;//参考上述定义宏掩码。
	float  fPowerRemainPercent;//剩余电池电量百分比
	BYTE   byPowerRemainMinute;//剩余电量可用时间,单位:分。
};

//CUNPACK_ZTI
typedef struct tagZTIInfo
{
	int type;
	union{
		zti_st zti_t;
		SelfCheckInfo selfcheck;
	}data;
}ZTIInfo, *lpZTIInfo;


//通信申请指令 DEVICE_TXA     
typedef struct tagTXAInfo
{
	int nLocalId;   //与外设相连的用户机ID号（新增）
	int  nUserId;   //此次通信的收信方地址。
	BYTE byCommCat; //通信类别，0-特快通信，1-普通通信。
	BYTE byCodeType;//传输方式，0-汉字，1-代码，2-混合。
	bool ifcmd;   //口令识别 0-通信，1-口令识别（新增）
	bool ifAnswer; //是否应答 当口令识别为0时，是否应答参数填全0  （新增）
	char chContent[1024];//通信内容.byCodeType ==0:每个汉字14bits,以计算机内码传输;byCodeType==1,每个bcd码占4bits，混发时，每个字节占8bits
	int  nContentLen;    //通信内容的字节数（电文长度）
	//         byCodeType ==1:每个代码以一个ASCII标示。        
	//         byCodeType ==2:电文内容首字母固定为A4。     
}TXAInfo, *lpTXAInfo;

//波束状态..
typedef struct tagBeamInfo
{
	BYTE  byResponseNo;
	BYTE  byTimeDiffNo;
	BYTE  byPower[10];
}BeamInfo, *lpBeamInfo;

typedef struct tagPowerStatus
{
	DWORD LocalID;
	BYTE  Power1;
	BYTE  Power2;
	BYTE  Power3;
	BYTE  Power4;
	BYTE  Power5;
	BYTE  Power6;
}PowerStatus, *lpPowerStatus;
typedef int(*pTFunMsgCallback)(int msgid, const void* msg, unsigned int param, int xtra);
class CDevice :public CSinker{
public:
	CDevice();
	~CDevice();
	BOOL   Open(protocalname_t pProtocalType, const int nPort, const int nBaudRate, const int nDataBits, const int nStopBits, BOOL bIfParity, const int nParity, const char* desc, pTFunMsgCallback callback, bool& bComSuccess);
	BOOL   IsOpened();
	BOOL   command(int msgid, void* msg, int msgblocklen);
	void   InitDevice();
	void   SendBSI();
	void   SendZTI();
	void   Update(const UINT uMsgId, const void * pMsgBuffer, int para) ;
	virtual void  Log(const char * pStrLogInfo){};
	BOOL IsPending(){ return FALSE;/* m_bPending;*/ }
	void SetPending(BOOL bPending){ m_bPending =  bPending = FALSE;}
	int  GetInStationFreq();//得到服务频度
	BOOL IsInit();
	BOOL IsNormal(); //是否可以发射
	BOOL IsOffline();//设备长时间收不到BSI信息，已经下？
	int GetLocalId();
	int GetCommBitsLen(BOOL bUrgent);
protected:
	void InitSelf();
	void ClearSelf();
	void Close();
	static DWORD WINAPI worker(void* pvoid);
private:
	pTFunMsgCallback m_cb;
	BOOL m_bOpen;
	BOOL m_bStop;
	BOOL m_bInit;
	BOOL m_bPending;
	ISearchExplainer* m_searchexp;
	IMessager*   m_messager;
	IConnection* m_connection;
	DWORD   m_thid; //线程ID
	HANDLE m_thread;
	ICIInfo* m_icInfo;//服务频度
	BOOL m_bNormal;//true设备正常，没有故障
	time_t  m_activetime;
	
	CRITICAL_SECTION m_cs;
	
};

#endif