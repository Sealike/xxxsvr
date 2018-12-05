#ifndef __HEAD_H__
#define __HEAD_H__

#define  _DLL_COMPILE__
#ifdef   _DLL_COMPILE__
#define  PROTOINTERFACE_EXPORTS
#endif

typedef  char*  outbuffer_t;
typedef  char*  inbuffer_t;
typedef  void*  outvoid_t;
typedef  char*  protocalname_t;

#ifndef  _WINDEF_
	typedef unsigned int   UINT;
	typedef unsigned long  DWORD;
	typedef unsigned char  BYTE;
	typedef int            BOOL;
	typedef unsigned short WORD;
	#define TRUE     1
	#define FALSE    0
#endif

/************************************************************************
 * message struct and msgid mapping    
 * command struct and cmdid mapping
 ************************************************************************/
#ifndef _DLL_COMPILE__
	enum{
		CUNPACK_NULL,
		CUNPACK_BSI = 1,//波束信息1
		CUNPACK_DWR = 2,//定位信息2
		CUNPACK_FKI = 3,//反馈信息3
		CUNPACK_ICI = 4,//
		CUNPACK_ICZ = 5,//
		CUNPACK_KLS = 6,//
		CUNPACK_HZR = 7,//
		CUNPACK_TXR = 8,//
		CUNPACK_SBX = 9,//
		CUNPACK_ZTI = 10,//
		CUNPACK_ZDA = 11,//
		CUNPACK_LZM = 12,//
		CUNPACK_WAA = 13,//
		CUNPACK_WBA = 14,//
		CUNPACK_BGJG = 15,//
		CUNPACK_GGA = 16,//
		CUNPACK_RMC = 17,//
		CUNPACK_GSV = 18,//
		CUNPACK_PMU = 19,//
		CUNPACK_ICM = 20,//
		CUNPACK_PKY = 21,//
		CUNPACK_ALM = 22,//
		CUNPACK_BSD = 23,//
		CUNPACK_BSQ = 24,//
		CUNPACK_BSX = 25,//
		CUNPACK_ZTG = 26,
		CUNPACK_GBS = 27,//
		CUNPACK_GLL = 28,
		CUNPACK_ECT = 29,
		CUNPACK_KLT = 30,
		CUNPACK_GSA = 31,//
		CUNPACK_GXM = 32,
		CUNPACK_GLZK = 33,		//
		CUNPACK_DWXX = 34,		//
		CUNPACK_TXXX = 35,		//
		CUNPACK_TXHZ = 36,		//
		CUNPACK_KLSB = 37,		//
		CUNPACK_DHSC = 38,		//
		CUNPACK_DSJG = 39,		//
		CUNPACK_SCSJ = 40,		//
		CUNPACK_ZBSC = 41,		//
		CUNPACK_GBSC = 42,		//
		CUNPACK_GYSC = 43,		//
		CUNPACK_ICXX = 44,		//
		CUNPACK_ZHQR = 45,		//
		CUNPACK_ZJXX = 46,		//
		CUNPACK_LZXX = 47,		//
		CUNPACK_SJXX = 48,		//
		CUNPACK_BBXX = 49,		//
		CUNPACK_XHXX = 50,		//
		CUNPACK_GLXX = 51,		//
		CUNPACK_FKXX = 52,		//
		CUNPACK_BSXX = 53,		//
		CUNPACK_HXXX = 54,		//
		CUNPACK_HJSC = 55,		//
		CUNPACK_ILXX = 56,		//
		CUNPACK_QLXX = 57,		//
		CUNPACK_RNCS = 58,		//
		CUNPACK_QUERYPOS = 59,	//
		CUNPACK_QUERYCOMM = 60,	//
		//------------通信协议扩展
		CUNPACK_MIXCOMMINFO = 61,//
		CUNPACK_POSREPORT = 62,	//
		CUNPACK_XSD = 63,//
		CUNPACK_XST = 64,//
		CUNPACK_DHV = 65,//
		CUNPACK_RNS = 66,
		CUNPACK_RPQ = 67,
		CUNPACK_TSQ = 68,
		CUNPACK_WZB = 69,
		CUNPACK_ZYB = 70,
		CUNPACK_APL = 71,
		CUNPACK_BID = 72,
		CUNPACK_ZHR = 73,
	};

	enum{
		DEVICE_GLJC  =1,
		DEVICE_DWSQ  =2,
		DEVICE_TXSQ  =3,
		DEVICE_DSSQ  =4,
		DEVICE_JSZL  =5,
		DEVICE_GBXX  =6,
		DEVICE_GYXX  =7,
		DEVICE_WMCS  =8,
		DEVICE_ICJC  =9,
		DEVICE_ZHSQ  =10,
		DEVICE_BSSZ  =11,
		DEVICE_BSCX  =12,
		DEVICE_GXZR  =13,
		DEVICE_GXDQ  =14,
		DEVICE_XTZJ  =15,
		DEVICE_LZSZ  =16,
		DEVICE_LZDQ  =17,
		DEVICE_SJSC  =18,
		DEVICE_XHDQ  =19,
		DEVICE_BBDQ  =20,
		DEVICE_JMSZ  =21,
		DEVICE_POSITIONREPORT = 22,

		//2.1new 
		DEVICE_BDBSS =23,
		DEVICE_BDCXA =24,
		DEVICE_BDRMO =25,
		DEVICE_BDPKY =26,
		DEVICE_BDGXM =27,
		DEVICE_BDTXA =28,
		DEVICE_BDWAA =29,
		DEVICE_BDWBA =30,
		DEVICE_WZBG  =31,
		DEVICE_BDKLS =32,
		DEVICE_BDKLT =33,
		DEVICE_BDJMS =34,
		DEVICE_BDLZM =35,
		DEVICE_BDICA =36,
		DEVICE_BDDWA =37,
		DEVICE_BDDSA =38,
		DEVICE_BDBSD =39,
		DEVICE_BDBSQ =40,
		DEVICE_BDBSX =41,
		DEVICE_BDLPM =42,
		DEVICE_BDMSS =43,
		DEVICE_BDGLS =44,
		DEVICE_BDRIS =45,
		DEVICE_BDZHS =46,
		DEVICE_BDECS =47,
		DEVICE_SATDEL=48,
		DEVICE_MODPPS=49,
		DEVICE_BDRNS =50,
		DEVICE_BDRPQ =51,
		DEVICE_BDAPL =52,
		DEVICE_SECPULSE = 53,
	};

	//波束状态..CUNPACK_BSI = 1
	typedef struct tagBeamInfo
	{
		BYTE  byResponseNo;
		BYTE  byTimeDiffNo;
		BYTE  byPower[10];
	}BeamInfo, *lpBeamInfo;

	//定位信息, 消息定义常量 CUNPACK_DWR = 2
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

	//速度信息，CUNPACK_FKI = 3
	typedef struct tagFKIInfo
	{   
		char chCommandName[5];//指令名称:如"DWT"
		char chExeRslt       ;//执行情况:'Y':执行成功,'N':执行失败。
		char chIntervalInd   ;//'Y':频度设置无误,'N':频度设置错误.
		BYTE byTranInhabitInd;//发射抑制指示.0:发射抑制解除，1:接收到系统的抑制指令，发射被抑制，2:由于电量不足，发射被抑制,3:由于无线电静默,发射被抑制。
		BYTE byMinuteWait;    //指令需要等待的时间,分部分
		BYTE bySecondWait;    //指令需要等待的时间,秒部分
	}FKIInfo, *lpFKIInfo;

	//IC信息 CUNPACK_ICI = 4
	typedef struct tagICIInfo
	{   
		int nLocalId ;
		int nSerialNo;
		int nBroadId ;     //对于普通用户机,为接收通播ID;对具备指挥功能的用户设备,为发送通播ID。
		int nUsrAttribute; //用户特征值:0:智慧型用户机1:一类用户机2:二类用户机3:三类用户机4:指挥型用户机(进行身份认证)5:一类用户机(进行身份认证)6:二类用户机(进行身份认证)7:三类用户机(进行身份认证)

		int  nServiceFreq; //单位:秒
		BYTE byCommLevel;  //通信等级:1-4
		char chEncriptFlag;//加密标识。'E':加密,'N':非密。
		int  nSubUsersNum; //下属用户。

	}ICIInfo, *lpICIInfo;
	 
	//指挥机下属子用户信息 CUNPACK_ICZ = 5
	typedef struct tagICZInfo
	{   
		int nTotalSubUserNums;
		int nUsrCntInFrm;
		int nSubUserIds[40];
	}ICZInfo, *lpICZInfo;

	//UTC时间，日期，和本地时区等信息 消息常量定义CUNPACK_ZDA  = 11
	typedef struct tagZDAInfo
	{
		int    nMode;//模式指示:1~9。1:RDSS定时结果,2:RNSS定时结果。
		WORD   wYear;//UTC datetime
		WORD   wMonth;
		WORD   wDay;
		WORD   wHour;
		WORD   wMinute;
		float  fSeconds;
		int    nTimeArea;    //0 ~ [13,-13];本地时区(小时加分钟(nLocalMinDiff),以及表示本地区的符号)加上本地时间即得UTC时间。
		int    nLocalMinDiff;//0~59,单位分钟。
		WORD   wModifyHour;  //定时修正时刻HHMMSS.SS
		WORD   wModifyMinute;
		float  fModifySeconds;
		double dfValueModify ;//修正值。单位:纳秒。
		int    nPrecisionFlag;//精度指示:0~3;0:未监测，1:0到10纳秒，2:10到20纳秒，3:大于20纳秒。
		char   chSatStatus;   //卫星信号锁定状态:'Y'/'N'。

	}ZDAInfo,*lpZDAInfo;

	//定位卫星及精度因子。。CUNPACK_GSA = 31
	typedef struct tagGSAInfo
	{
		char   chMode;//模式指示:M手动，A自动。
		int    nModeOption;//1~3,1:定位不可用,2:2D，3:3D
		int    nSatNo[12];//北斗PRN号:1~32,GPS:33~70 GLONASS : 71~102
		float  fPdop;
		float  fHdop;
		float  fVdop;
		float  fTdop;

	}GSAInfo,*lpGSAInfo;

	//速度信息。。CUNPACK_DHV = 65
	typedef struct tagDHVInfo
	{
		BYTE  byHour;
		BYTE  byMinute;
		float fSeconds;

		float fVel;//速度
		float velX;//X轴速度
		float velY;
		float velZ;
		float velGrnd;//地面速度
		float velMax;//最大速度
		float velAve;//平均速度
		float velCourseAve;//全程平均速度
		float velValid;
		char  chUnit;//单位，推荐为米	
	}DHVInfo,*lpDHVInfo;	

	//通信信息常量定义 : CUNPACK_TXR = 8
	typedef struct tagTXRInfo
	{   
		BYTE byInfoType;//信息类别:1~5。1:普通通信，2:特快通信，3:通播通信,4:按最新存入电文查询获得的通信,5:按发信方地址查询获得的通信。
		int  nLocalID  ;//发信方地址
		int  nDestionID;
		BYTE byTeleFormat;//电文形式:0-汉字，1-代码，2-混合传输
		BYTE bySendHour;  //发信时间(时)。byInfoType == 1，2，3时，发信时间无效。byInfoType ==4，5时,为被查询的通信电文在中心控制系统记录的发送时间。
		BYTE bySendMinute;//发信时间(分)。
		char chContent[1024];//通信内容。C串。当byTeleFormat == 2时，电文内容首字母固定为A4.
		int  nContentLen;
		BYTE byCRC;
	}TXRInfo, *lpTXRInfo;
	
	//通信申请指令 DEVICE_TXA     
	typedef struct tagTXAInfo
	{   
		int nLocalId;   //与外设相连的用户机ID号（新增）
		int  nUserId;   //此次通信的收信方地址。
		BYTE byCommCat; //通信类别，0-特快通信，1-普通通信。
		BYTE byCodeType;//传输方式，0-汉字，1-代码，2-混合。
		bool ifcmd;   //口令识别 0-通信，1-口令识别（新增）
		bool ifAnswer; //是否应答 当口令识别为0时，是否应答参数填全0  （新增）
		char chContent[1024];//通信内容.byCodeType ==0:每个汉字16bits,以计算机内码传输;
		int  nContentLen;    //通信内容的字节数（电文长度）
		//         byCodeType ==1:每个代码以一个ASCII标示。        
		//         byCodeType ==2:电文内容首字母固定为A4。     
	}TXAInfo, *lpTXAInfo;

	//CUNPACK_BDDWA 定位申请
	typedef struct tagBDDWA_STRUCT
	{
		int    userAddr;  //用户地址

		BOOL   bifUrgency;//紧急指示
		BYTE   bPosType  ;//0:有高程1：无高程，2：测高1，3测高2。
		BOOL   bifHigh   ;//是否高空用户

		double dfHgt     ;  //高程数据  单位:米
		float  fAntennaHgt; //天线高。  单位:米
		double fAirPressure;//气压数据。单位:帕
		double fTemperature;//温度数据。单位:度
		WORD   wFreq;       //定位频度。单位:秒,0:单次定位。
	}BDDWA_STRUCT,*pBDDWA_STRUCT,DWAInfo,*lpDWAInfo;

	//位置报告1双向语句 ：DEVICE_BDWAA = 29
	typedef struct tagWAAInfo
	{   
		BYTE   byInfoType;//信息类型。0-表示为用户设备接收的位置报告信息，此时nReportFreq无效。nUserId为发送位置报告的用户的地址。
		//          1-表示为用户设备发送的位置报告信息，此时nUserId为接收位置报告信息的用户地址。
		int    nReportFreq; //报告频度，单位:秒。
		int    nUserId;
		WORD   wReportHour;//位置报告时间信息
		WORD   wReportMinute;
		float  fReportSecond;
		double dfLat;//纬度，单位:度。
		char   chLatDir;//'N'/'S';
		double dfLong;//经度，单位:度。
		char   chLongDir;//'E'/'W';
		double dfHgt;//高程
		char   chHgtUnit;//高程单位。
	}WAAInfo, *lpWAAInfo;

	////CUNPACK_ZTI = 10
	typedef struct tagZTIInfo
	{   
		//#define MASK_CHARGE       0x01   0:充电状态 ，1:非充电状态
		//#define MASK_POWER_INOUT  0x02   0:内置电池 ，1:外置电源
		//#define MASK_CHANNEL      0x04   0:正常，1：异常
		//#define MASK_ANTENNA      0x08   0:正常，1：异常
		//#define MASK_ENCRYPMODULE 0x10   0:正常，1：异常
		//#define MASK_PRMMODULE    0x20   0:正常，1：异常

		BYTE   byStatusMask;//参考上述定义宏掩码。
		float  fPowerRemainPercent;//剩余电池电量百分比
		BYTE   byPowerRemainMinute;//剩余电量可用时间,单位:分。
	}ZTIInfo, *lpZTIInfo;

	typedef struct  tagSatInfo
	{   
		int   nSatNo;      
		float fElevation;  //卫星仰角   单位:度
		float fOrientAngle;//卫星方位角 单位:度
		float fSnr;        //卫星信噪比 单位:dB-Hz

	}SatInfo, *LPSatInfo;

	//可见卫星信息CUNPACK_GSV = 18
	typedef struct  tagGSVInfo
	{   
		char    chSys[3];//BD+'\0', GP+'\0', GL+'\0', GA+'\0'.
		int     nSatVisible;
		SatInfo satInfo[4];
		int     nTotalStence;
		int     nCurrStence;
		int     nCurrSatNum;
	}GSVInfo, *LPGSVInfo;

	//RNSS 定位测速信息 CUNPACK_RMC = 17
	typedef struct  tagRMCInfo
	{
		char    chSys[3];//BD+'\0', GP+'\0', GL+'\0', GA+'\0',GN+'\0'.
		int     nYear ;
		int     nMonth;
		int     nDay  ;
		int		nPosHour;
		int		nPosMinute;
		float	fPosSecond;
		BYTE    byPosStatus;//状态指示A/V;
		double  dfLong   ;//度。
		double  dfLat    ;//度。
		char    chLatNS  ;//纬度方向 'N'/ 'S'。
		char    chLongEW ;//经度方向 'E'/ 'W'。
		float   fGrndVel ;//地面速度，单位：节。
		float   fGrndDir ;//地面航向，单位：度，以真北为参考基准，沿顺时针方向至航向的角度。
		float   fMagAngle;//磁偏角
		char    chMagDir ;//磁偏角方向
		char    chModeFlag;//A:自主定位，D:差分，E:估算，N:数据无效。
	}RMCInfo, *LPRMCInfo;

	//GNSS定位结果 CUNPACK_GGA = 16
	typedef struct  tagGGAInfo
	{ 
		char    chSys[3];//BD+'\0', GP+'\0', GL+'\0', GA+'\0',GN+'\0'.
		int		nPosHour;
		int		nPosMinute;
		float	fPosSecond;
		double  dfLong ;  //度
		double  dfLat  ;  //度
		char    chLatNS;  //纬度方向 N S
		char    chLongEW; //经度方向 E W
		BYTE    byStatus; //状态指示0-8 a
		BYTE    bySatNum; //卫星数 两位
		double  dfGndHigh;//大地高    
		char    chGndHighUint;//大地高  单位；
		double  dfHgtExcept;//高程异常          
		char    chHgtExceptUint;   //高程异常单位M*/
		unsigned short   DiffDate; //差分数据龄期 xxxx
		char    chDiffStation[128];//差分站台ID    
		float   fHdop;   
		double  dfVdop;
	}GGAInfo, *LPGGAInfo;

	//数据输出控制指令 DEVICE_BDRMO = 25
	typedef struct tagRMOInfo
	{   
		char chName[4];//合法的标识串，如"GGA"
		BYTE byMode;   //模式。1:关闭指定语句，2:打开指定语句，3:关闭全部语句，4:打开全部语句。byMode == 3或byMode ==4时:目标语句数据保留区为空。
		int  nOutFreq; //单位:秒。byMode == 4时,此保留区域为空。
	}RMOInfo,*lpRMOInfo;

/************************************************************************
 * c++ type interface    
 * 
 ************************************************************************/
	class  CSinker{
	public:
		virtual void  Update(const UINT uMsgId,const void * pMsgBuffer,int para) = 0;
		virtual void  Log(const char * pStrLogInfo) = 0;
	};
	
	class IMessager
	{
	public:
		virtual int  Advise  (CSinker* pSinker) = 0 ;
		virtual int  Unadvise(const CSinker* pSinker) = 0 ;
	};	

	class ISearchExplainer{
	public:
		virtual int  ProcessData(const BYTE byBuffData[],const int nDataLen) = 0 ;
		virtual int  EncodeFrame(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen) = 0;
		virtual void Release() = 0;
		virtual BOOL AddExtenedPlugin(int msgid,char* title,pCBExt_explain exp_cb)= NULL;
	};

#endif

#endif