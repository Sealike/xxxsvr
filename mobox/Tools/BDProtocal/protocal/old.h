#ifndef __STRUCT_H
#define __STRUCT_H
//////////////////////////////////////////////////////////////////////////
//此文件包含所有信息类型，所作改动必须同步更改DllInterface.h
//////////////////////////////////////////////////////////////////////////
#include "ConstDefine.h" 

typedef unsigned int   UINT;
typedef unsigned char  BYTE;
typedef unsigned long  DWORD;
typedef unsigned short WORD;

typedef struct tagSInfoTitle
{
	int iInfoID;
	char cTitle[15];

}SInfoTitle;

//波束状态..
typedef struct tagBeamInfo
{
	BYTE  byResponseNo;
	BYTE  byTimeDiffNo;
	BYTE  byPower[10];
}BeamInfo, *lpBeamInfo;

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

//IC信息 CUNPACK_ICI
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


typedef struct tagKLSInfo
{   
	int  nSubUsrId;
	char chResponseFlag;

}KLSInfo, *lpKLSInfo;

//双向语句,指挥型用户设备输出下属用户发送的口令识别内容，或者普通设备响应指挥型用户设备口令识别指令的信息。
typedef struct tagKLTInfo
{   
	char chCharacter;//'P'/'Z'。'P':普通型用户设备响应指挥型用户设备口令识别指令的信息。
	                 //         'Z':指挥型用户设备输出下属用户发送的口令识别内容。
	int  nUsrId;     //chCharacter==P:为接收口令识别的上级指挥型用户机，'Z':用户地址为下属用户地址。
	BYTE byCommType; //0:汉字，1:代码,2：混发。
	char chContent[512];//电文内容。

}KLTInfo, *LPKLTInfo,KLTStruct,*LPKLTStruct;

//消息常量: CUNPACK_HZR;
typedef struct tagHZRInfo
{   
	int  nLocalId;       //用户地址。
	BYTE byResponseNum;  //回执数  :0 ~ 5;0:表示无回执。
	WORD wSendHour[5];   //对应回执1~5的发信时间
	WORD wSendMinute[5];
	WORD wAckHour[5];	 //对应回执1~5的回执时间
	WORD wAckMinute[5];

}HZRInfo, *lpHZRInfo;


typedef struct tagPMUInfo
{   
	BYTE byFreq;  //频点号:1,2,3
	WORD wBgnYear;//开始日期
	WORD wBgnMonth;
	WORD wBgnDay;
    
	WORD wEndYear;//结束日期
	WORD wEndMonth;
	WORD wEndDay;

}PMUInfo, *LPPMUInfo;

//CUNPACK_ICM
typedef struct tagICMInfo
{   
	WORD wEndYear;//密钥有效起始日期暂时无用
	WORD wEndMonth;
	WORD wEndDay;

	WORD wBgnYear;//密钥有效结束日期
	WORD wBgnMonth;
	WORD wBgnDay;
	
}ICMInfo, *LPICMInfo;

//
typedef struct tagPKYInfo
{   
	UINT uWeek;         //周数，0 :本周。
	char chSecretKey[7];//密钥
	char chKey[129];    //KEY
	char chB1Param[25]; //B1工作参数
	char chB2Param[25]; //B2工作参数
	char chB3Param[25]; //B3工作参数
    
}PKYInfo, *LPPKYInfo;

//CUNPACK_BSD;
typedef struct tagBSDInfo
{   
	char chName[21];    //标识名称;
	char chMapName[21]; //图标名称;
	double dfLat;       //纬度。单位：度；
	char   chLatNS;     //纬度方向。'N'/'S'
	double dfLong;      //经度。单位:度。
	char   chLongEW;    //经度方向。'E'/'W'
	double dfHgt;       //高程。
	char   chHgtUnit;   //高程单位;
}BSDInfo, *LPBSDInfo;
//CUNPACK_BSQ;
typedef struct tagBSQInfo
{   
	char chName[21];    //标识名称;
	double dfCenterLat; //中心点纬度,单位:度。
	char   chLatNS;     //纬度方向。'N'/'S'
	double dfCenterLong;//中心点经度,单位:度
	char   chLongEW;    //经度方向。'E'/'W'
	double dfCenterHgt; //中心点高程。
	char   chHgtUnit;   //高程单位;
	double dfRadius;    //区域半径
	char   chRadiusUnit;//半径单位
}BSQInfo, *LPBSQInfo;


typedef struct tagPositionInfo
{   
	double dfLat;       //纬度。单位：度；
	char   chLatNS;     //纬度方向。'N'/'S'
	double dfLong;      //经度。单位:度。
	char   chLongEW;    //经度方向。'E'/'W'
	double dfHgt;       //高程。
	char   chHgtUnit;   //高程单位;

}PositionInfo, *LPPositionInfo;

//CUNPACK_BSX;DEVICE_BSX，双向语句。
typedef struct tagBSXInfo
{   
	char chName[21];    //标识名称;
	WORD wTotalSentence;
	WORD wCurrSentenceNo;
	WORD wPointNum;
	tagPositionInfo posInfo[100];

}BSXInfo, *LPBSXInfo;

//输出语句，描述航向和航迹; CUNPACK_ZTG

typedef struct tagVTGInfo
{   
	float fGrndDirNorth;//对地方向，参考真北。
	char  chNorthRef;   //真北参考。
	float fGrndDirMagNorth;//对地方向，参考磁北。
	char  chMagNorthRef;//磁北参考。
	float fGrndSpeedN;  //以节描述的地速
	char  chSpeedUnitN; //速度单位：节
	float fGrndSpeedKm; //以千米描述的地速
	char  chSpeedUnitK; //速度单位：km/h;
	char  chModeIndicate;//A:自主模式,B:差分模式,E:估算模式,M:手动输入模式,S:模拟器模式,N:数据无效。

}VTGInfo, *LPVTGInfo;

//对应的常量 CUNPACK_GBS
typedef struct tagGBSInfo
{   
	char  chSysType[3];//GN , GP, GA,BD,GL,.
	WORD  wPosHour  ;//定位时间(UTC)
	WORD  wPosMinute;
	float fPosSecond;
	
	float fLatPredicErr;//纬度预估误差,单位:米。
	float fLonPredicErr;//经度预估误差,单位:米。
	float fHgtPredicErr;//高程预估误差,单位:米。
    
	BYTE  byFailedSat;//估计出错的卫星号。当使用多个卫星系统时，北斗卫星号范围为1~32,GPS卫星号范围为33~70; 
    float fProbability;//在估算时，漏检卫星的概率。
    
	float fPredictSatCauseErr;//出错卫星的估计偏差。
	float fPredictStdErr;//偏置估算的标准偏差。
	
}GBSInfo, *LPGBSInfo;

//对应的常量 CUNPACK_ECT
typedef struct tagECTInfo
{   
	char  chSysType[3];//GN , GP, GA,BD,GL,.
	BYTE  bySatNo;//卫星号.
	char  chFreq[3];//频点信息:"B1","B2","B3","S".
	BYTE  byChannelNo;//通道号。
	char  chRoute;//支路:'I','Q';
	char  chTeleInfo[256];//原始导航电文信息。
	WORD  wTeleLen;//电文长度(字符数);
	
}ECTInfo, *LPECTInfo;

//对应的常量 CUNPACK_GLL
typedef struct tagGLLInfo
{   
	char   chSysType[3];//GN , GP, GA,BD,GL,.
	double dfLat;      //纬度.单位：度
	char   chLatNS;    //纬度方向：'N'/'S'
	double dfLon;      //经度。单位：度。
	char   chLonEW;    //经度方向:'E'/'W'
	WORD   wPosHour;   //定位时刻:UTC.
	WORD   wPosMinute;
	float  fPosSecond;
    char   chDataStatus;//数据状态:A-有效；V-无效。
	BYTE   byModeIndicate;//模式指示:0~5;0-自动模式，1-差分模式，2-估算，3-手动输入模式，4-模拟器模式。

}GLLInfo, *LPGLLInfo;
//DEVICE_MSS，输入语句。设置用户设备当前定位方式。
//当工作模式(chModeCorZ)取'C'时，byTypeItem代表的测试项目具体如下:
//0:误码率，1:定位，2:冷启动,3:温启动,4:热启动,5:测距，6:定时，7:重捕,8:raim,9:位置报告
//当工作模式(chModeCorZ)取'Z'时，byTypeItem代表的测试项目具体如下:
//1:RNSS单频定位,2:RNSS双频定位,3:RDSS定位,4:GPS定位，5:兼容定位
//对于BD-2系统，频点取值为:"B1","B2","B3","S";对于GPS系统，频点取值为"L1".
//支路(chRoute_):"C":C码, "P":P码, "A":全部支路。
typedef struct tagMSSInfo
{   
	char chModeCorZ;    //'C':测试模式;'Z':正常模式
	BYTE byTypeItem;    // 定位模式/测试项目。
	char chFreq [3][3];    //B1、2、3。S,L1; *+'\0'形成c的串。
	char chRoute[3][3];    //C,P,A。* +'\0'形成c的串。
	char chSys[3][3];      //BD,GP,GL.
	
}MSSInfo, *LPMSSInfo;

//DEVICE_BDPMI，输入语句，设置用户设备工作在省电模式。
typedef struct tagLPMInfo
{   
	BYTE byPowerSavingFlag;//0:省电模式,1:正常模式。

}LPMInfo, *LPLPMInfo;

//CUNPACK_ZDA;
//UTC时间，日期，和本地时区等信息 消息常量定义CUNPACK_ZDA 
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

//CUNPACK_GSA;
//定位卫星及精度因子。。
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

//CUNPACK_GXM;
//管理信息。。双向语句
typedef struct tagGXMInfo
{
	int  nType; //1:设置用户设备管理信息 2:读取用户设备管理信息 3:用户设备返回的管理信息 
	int  nSerialNo;
	char chMagInfo[65];//16进制ASCII码，指令类型为2时，管理信息为空。
	
}GXMInfo,*lpGXMInfo;

//CUNPACK_DHV;
//速度信息。。
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

typedef struct tagFKIInfo
{   
	char chCommandName[4];//指令名称:如"DWT"
	char chExeRslt       ;//执行情况:'Y':执行成功,'N':执行失败。
	char chIntervalInd   ;//'Y':频度设置无误,'N':频度设置错误.
	BYTE byTranInhabitInd;//发射抑制指示.0:发射抑制解除，1:接收到系统的抑制指令，发射被抑制，2:由于电量不足，发射被抑制,3:由于无线电静默,发射被抑制。
	BYTE byMinuteWait;    //指令需要等待的时间,分部分
	BYTE bySecondWait;    //指令需要等待的时间,秒部分
}FKIInfo, *lpFKIInfo;

typedef struct tagICZInfo
{   
	int nTotalSubUserNums;
	int nUsrCntInFrm;
	int nSubUserIds[40];
}ICZInfo, *lpICZInfo;

//通信信息常量定义 : CUNPACK_TXR 
typedef struct tagTXRInfo
{   
	BYTE byInfoType;//信息类别:1~5。1:普通通信，2:特快通信，3:通播通信,4:按最新存入电文查询获得的通信,5:按发信方地址查询获得的通信。
	int  nLocalID  ;//发信方地址
	BYTE byTeleFormat;//电文形式:0-汉字，1-代码，2-混合传输
	BYTE bySendHour;  //发信时间(时)。byInfoType == 1，2，3时，发信时间无效。byInfoType ==4，5时,为被查询的通信电文在中心控制系统记录的发送时间。
	BYTE bySendMinute;//发信时间(分)。
	char chContent[1024];//通信内容。C串。当byTeleFormat == 2时，电文内容首字母固定为A4.
	int  nContentLen;
}TXRInfo, *lpTXRInfo;

//双向语句，设备零值信息、读取设备零值指令，设置设备零值指令。
//对应的命令/消息常量：DEVICE_BDLZM/CUNPACK_LZM。
typedef struct tagLZMInfo
{   
	BYTE   byManageMode;//1-读取设备零值，2-设置设备零值，3-返回设备零值。
	double dfZeroValue ;//byManageMode == 1时，设备零值为空。

}LZMInfo, *LPLZMInfo;

//通信申请指令 DEVICE_TXA     
typedef struct tagTXAInfo
{   
	int  nUserId;   //此次通信的收信方地址。
	BYTE byCommCat; //0-特快通信，1-普通通信。
	BYTE byCodeType;//0-汉字，1-代码，2-混合。
	char chContent[1024];//通信内容.byCodeType ==0:每个汉字16bits,以计算机内码传输;
	int  nContentLen;    //通信内容的字节数。
	//         byCodeType ==1:每个代码以一个ASCII标示。        
	//         byCodeType ==2:电文内容首字母固定为A4。     
}TXAInfo, *lpTXAInfo;

//位置报告1双向语句 ：DEVICE_WAA
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

//位置报告2 入站语句。消息常量定义 CUNPACK_WBA 
typedef struct tagWBAInfo
{   
	int    nUserId  ;  //接收位置报告方的用户地址。
	char   chHgtType;  //高程指示:'H'-高空;'L'普通。
	double dfAnteHgt;  //天线高。单位:米。
	float  fReportFreq;//报告频度:秒。
}WBAInfo, *lpWBAInfo;

//设备相关参数,消息常量定义 CUNPACK_SBX
typedef struct tagSBXInfo
{   
	char   chManufac[128];//设备供货商名称
	char   chDevType[128];//设备类型
	char   chSoftVer[128];//程序版本号
	char   chAppProtocalVer[128];//串口协议版本号
	char   chICDVer[128];//ICD协议版本号
	char   chDevSerialNo[128];//设备序列号
	int    nUserId;//ID号
}SBXInfo, *lpSBXInfo;

////CUNPACK_ZTI
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

////CUNPACK_GSV
typedef struct  tagGSVInfo
{   
	char    chSys[3];//BD+'\0', GP+'\0', GL+'\0', GA+'\0'.
	int     nSatVisible;
	SatInfo satInfo[4];
	int     nTotalStence;
	int     nCurrStence;
	int     nCurrSatNum;
}GSVInfo, *LPGSVInfo;
//CUNPACK_RMC
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

typedef struct tagWZBGgx{
	UINT  uLocalId;
	BOOL  bOn;
	UINT  uRcvId;
	UINT  uFreq;//0表示单次位置报告
	BYTE  byType;//0： 通信	1：定位
	SYSTEMTIME T;
	double dfLat;
	double dfLon;
	double dfHgt;
	double dfAnteHgt;
}WZBGgx,*lpWZBGgx;

//输入语句，设置用户输出原始导航信息，对应的命令常量 :DEVICE_BDECS
typedef struct tagECSStruct
{
	char chFreq[3];  //频点号。"B1","B2","B3","S"。
	BYTE byChannelNo;//通道号。"01"~"12"。
	char chRoute[3] ;//  支路。'I','Q','A'。
}ECSStruct ,*LPECSStruct ;

//DEVICE_BDGLS
typedef struct tagGLSStruct
{
	double dfLat;    //纬度 单位：度
	char   chLatNS;  //纬度方向:'N'/'S';
	double dfLon;    //经度 单位：度。
	char   chLonEW;  //纬度方向:'E'/'W';
	float  fHgt;     //高程.参考大地水准面。
	char   chHgtUnit;//高程单位。
	char   chPrecisionIndicat;//精度指示.'A'/'P';'A'-概略位置，空间误差介于1m~100km之间；'P'-精确位置，空间误差小于1米。
	int    nYear;    //当前的UTC日期时间。
	WORD   wMonth;
	WORD   wDay;
	WORD   wHour;
	WORD   wMinute;
	WORD   wSecond;
	BYTE   byInitCat;//0-数据有效，设置为温/热启动，1-清除星历，设置为温启动，2-清除存储器，设置为冷启动；
}GLSStruct, *lpGLSStruct;

//用户机复位指令。
//输入语句:  命令定义常量:DEVICE_BDRIS;
typedef struct tagRISStruct
{   
	DWORD dwReserved1;//not used!
	DWORD dwReserved2;//not used!
}RISStruct, *LPRISStruct;

//用户机自毁指令。
//输入语句:  命令定义常量:DEVICE_BDZHS;
typedef struct tagZHSStruct
{   
	DWORD dwCmd;//固定常量:0xAA5555AA,
}ZHSStruct, *LPZHSStruct;

/*
#define __BD   0 //北斗系统
#define __GPS  1 //GPS系统
#define __GLO  2 //Glonass系统
#define __GAL  3 //伽利略系统

#define __B1   0
#define __B2   1
#define __B3   2
#define __L1   3

#define NONE   0 
#define __C    1
#define __P    2
#define __A    __C
*/
typedef struct tagSatDelete
{   
	int   nSatNo;//卫星号
	int   nFreq; //频点 0~3
	int   nSys;  //系统 0~3
	BYTE  byOper;//0:恢复卫星 1:剔除卫星；
}SatDelete,*LPSatDelete;

typedef struct tagOnePPSCorr
{   
	int    nSys;   //系统 0~3
	double dfValue;//零值，单位：秒；
}OnePPSCorr,*LPOnePPSCorr;

//功率检测
//#define DEVICE_GLJC  1
typedef struct tagPowerCheck
{
	DWORD LocalID;
	BYTE  Freq;			//单位：分钟，0:单次输出
}PowerCheck, *lpPowerCheck;

//北斗定位请求
//#define DEVICE_DWSQ  2
typedef struct tagPosReq
{   
	DWORD LocalID;
	BOOL  ifUrgent;		//0:否  1:紧急定位
	BOOL  ifTimeDiff;	//有无时差[固定填0]
	BYTE  MeasureMode;	//00:有高程  01:无测高  10:测高1  11:测高2
	BOOL  ifSecret;		//保密[固定填零]

	BOOL  ifHighAlt;	//[标准协议2.1版无此项] 高程指示[0:普通  1:高空]
	int	  Altitude; 	//高程[单位:米]
	DWORD Antena;		//天线高
	DWORD Pressure;		//[标准协议2.1版无此项] 气压数据[无符号，单位:0.1Hp]
	short Temperature;	//[标准协议2.1版无此项] 温度[单位:0.1度]

	WORD PosFreq;		//秒
}PosReq, *lpPosReq;

//通信请求
//#define DEVICE_TXSQ  3
typedef struct tagCommReq
{	
	DWORD LocalID;
	DWORD DestAddress;		//接收方地址
	DWORD InfoLen;			//数据长度(单位：Bit)  不能传字节数，因为BCD可能不是整字节
	BYTE  InfoBuff[420+1];	//汉字:Max 120个  BCD:Max 420
	BYTE  GraphOrQuery;		//010:报文通信  011:查询通信
	//报文通信
	BOOL  ifSecret;			//固定填零
	BOOL  ifQuick;			//00:普通   01:特快[注：和协议规定相反]
	BOOL  ifBCD;			//0:汉字    1:代码
	BOOL  ifCmd;			//[标准协议2.1版无此项] 口令识别[0:通信  1:口令识别]
	BOOL  ifAnswer;			//[标准协议2.1版无此项] 是否应答
	//查询通信
	BOOL  QueryType;		//0:定位查询 1:通信查询
	BYTE  QueryMode;		//对应定位查询:  00: 1次  01: 2次  10: 3次
	//对应通信查询:  10:回执查询  01:按发信地址  00:按最新存入时间

	BYTE ifANS;				//装机所扩展协议，自由文电中的是否应答字段
}CommReq, *lpCommReq;

//位置上报接收
typedef struct tagPositionReportRecv
{
	DWORD LocalID ;   
	DWORD SrcAddress;

	//时间
	BYTE Hour ;
	BYTE Minute ;
	BYTE Second ;
	BYTE Msel10 ;  //十毫秒为单位
	//经度
	BYTE LDegree ;
	BYTE LMinute ;
	BYTE LSecond ;
	BYTE LDecsec ;
	//纬度
	BYTE BDegree ;
	BYTE BMinute ;
	BYTE BSecond ;
	BYTE BDecsec ;

	//定位方式
	BYTE PosType ;
	int Alt ;
	int EF ;

}PositionReportRecv,*lpPositionReportRecv;

//定时申请
//#define DEVICE_DSSQ  4
typedef struct tagTimeReq
{
	DWORD LocalID;
	BOOL  ifHavePos;	//0:无概略位置 1:有概略位置
	BYTE  byType;
	//位置数据 [2.1]
	WORD  Long;	//[单位:0.1度]
	WORD  Lat;	//[单位:0.1度]
	//位置数据 [3.0]
	BYTE  Long_deg;
	BYTE  Long_min;
	BYTE  Long_sec;
	BYTE  Long_decsec;
	BYTE  Lat_deg;
	BYTE  Lat_min;
	BYTE  Lat_sec;
	BYTE  Lat_decsec;
	int   Altitude;
	int   UniZero;	//单向零值[=本机零值+固定值193576ns]
	int   AppendZero;	//附加零值
	BYTE  TimeSys;	//输出设定[00: 北斗时  01: UTC]

	WORD ServeFreq;//入站频度  (单位:s)
}STimeReq, *lpSTimeReq;

//结束指令
//#define  DEVICE_JSZL  5
typedef struct tagEndInstruct
{
	DWORD LocalID;
	DWORD dwVal;
}EndInstruct, *lpEndInstruct;

//广播信息
//#define  DEVICE_GBXX  6
typedef struct tagBroadInfo
{
	DWORD LocalID;
}BroadInfo, *lpBroadInfo;

//公用信息
//#define DEVICE_GYXX  7
typedef struct tagPublicInfo
{
	DWORD LocalID;
}PublicInfo, *lpPublicInfo;

//误码测试
//#define  DEVICE_WMCS  8
typedef struct tagErrCodeTest
{
	DWORD LocalID;
	BYTE  IQFrame;	//00:输出I路数据  01:输出Q路数据
	BYTE  BeamNo;	//001: 1波束  010: 2波束  011: 3波束 100: 4波束  101: 5波束  110:6波束
}ErrCodeTest,*lpErrCodeTest;

//身份认证
//#define  DEVICE_ICJC  9
typedef struct tagReadCardReq
{
	DWORD LocalID ;
	BYTE  FrameNo ;
}ReadCard,*lpReadCardReq;

//主动自毁
//#define  DEVICE_ZHSQ  10
typedef struct tagSelfDes
{
	DWORD LocalID;
}SelfDes, *lpSelfDes;

//波束设置
//#define  DEVICE_BSSZ  11
typedef struct tagBeamSet
{
	DWORD LocalID ;
	BYTE AnswerBeam ;		//响应波束
	BYTE TimeDiffBeam;		// 0:自动选择，1~6 为1~6波束。
} BeamSet,*lpBeamSet ;

//波束查询
//#define DEVICE_BSCX   12
typedef struct tagBSCX_STRUCT
{
	int userAddr;
}BSCX_STRUCT,*pBSCX_STRUCT;

//序号管信设置
//#define DEVICE_GXZR   13
typedef struct tagCardSerialSet
{
	DWORD LocalID ;
	DWORD ICCardSerial ;		//序列号
	BYTE  ICManageInfo[65] ;	    // 管理信息
} CardSerialSet,*lpCardSerialSet;

//序号读取
//#define  DEVICE_GXDQ  14 
//#define  DEVICE_XHDQ  19
typedef struct tagserielRead
{
	DWORD LocalID ; 
}SerielRead,*lpserielRead; 

//系统自检
//#define  DEVICE_XTZJ  15
typedef struct tagSelfCheckReq
{	
	DWORD LocalID ;
	WORD  freq ;
}SelfCheckReq,*lpSelfCheckReq;

//零值设置
//#define  DEVICE_LZSZ  16
typedef struct tagZeroSet 
{
	DWORD LocalID ;
	DWORD ZeroValue ;	//零值
} ZeroSet,*lpZeroSet;

//零值读取
//#define  DEVICE_LZDQ  17
typedef struct tagZeroRead
{
	DWORD LocalID ;
} ZeroRead,*lpZeroRead;

//时间输出
//#define  DEVICE_SJSC  18
typedef struct tagOutTime
{
	DWORD LocalID ;
	WORD freq ;
}OutTime, *lpOutTime ;

//版本读取 
//#define  DEVICE_BBDQ  20
typedef struct tagEditionRead
{   
	DWORD LocalID ;
}EditionRead,*lpEditionRead;

//静默设置
//#define DEVICE_JMSZ   21
typedef struct tagSilent
{
	DWORD LocalID;
	BYTE  bSilent;
}Silent,*lpSilent;

//位置上报发送
//#define  DEVICE_POSITIONREPORT  22
typedef struct tagPositionReport
{
	BOOL bReport2;//新添加，是否是位置报告2.
	DWORD LocalID ;
	DWORD DestAddress ;

	BYTE isHight ;
	int Alt ;
	int EF ;

	//时间
	BYTE Hour ;
	BYTE Minute ;
	BYTE Second ;
	BYTE Msel10 ;  //十毫秒为单位
	//经度
	BYTE LDegree ;
	BYTE LMinute ;
	BYTE LSecond ;
	BYTE LDecsec ;
	//纬度
	BYTE BDegree ;
	BYTE BMinute ;
	BYTE BSecond ;
	BYTE BDecsec ;
}PositionReport,*lpPositionReport;
//时差输出
typedef struct tagTimeDiff
{
	DWORD LocalID;
	WORD  ServeFreq;//单位: 31.25ms
}TimeDiff, *lpTimeDiff;

//时钟读取信息
typedef struct tagSZDQInfo
{
	DWORD LocalID ;
}SZDQInfo,*lpSZDQInfo;

//串口输出
typedef struct tagAsyComSet
{
	DWORD LocalID;
	BYTE  ComRate;	//串口速率(00: 19.2Kbps  01: 1.2  02: 2.4  03: 4.8  04: 9.6  05: 38.4[Default]  06:57.6  07:115.2) 
}AsyComSet, *lpAsyComSet;

//======二炮双系统补充协议=======//

//GPS定位请求
typedef struct tagGpsReq
{
	DWORD LocalID;
	BYTE  Coordinate;	//[仅二炮双系统协议] 0:WGS84  1:BJ54
	BYTE  PosFreq;	//[仅二炮双系统协议] 0:输出一次  1～254:每1～254输出一次  255:不输出
}GpsReq, *lpGpsReq;

//======扩展协议=======//

//子用户上报请求
typedef struct tagChildrenReq
{
	BYTE  ControlCode;	//1：表示指挥型用户机向PC平台进行一次子用户上报
}ChildrenReq, *lpChildrenReq;

//链路维护
typedef struct tagLoopCheck
{
	BYTE  SequenceNum;//发送顺序号
}LoopCheck, *lpLoopCheck;

typedef struct tagKJBG
{ 
	int userAddr;
	BOOL bOn;
}KJBG_STRUCT,*pKJBG_STRUCT;

typedef struct tagRNQQ
{ 
	int userAddr;
	BYTE byWeek;
	int nPrmID;
}RNQQ_STRUCT,*pRNQQ_STRUCT;


//网卡设置
typedef struct tagNetCardSet
{
	BYTE  MACAddr[6];	//用户机网卡物理地址
	DWORD LocalIPAddr;	//PC平台IP地址
	WORD  LocalPort;	//PC平台端口号
	DWORD PeerIPAddr;	//用户机IP地址
	WORD  PeerPort;		//用户机端口号
	DWORD SubNetMask;	//子网掩码
}NetCardSet, *lpNetCardSet;



//======通信扩展协议=======//

//长电文切分
typedef struct tagGraphSplitReq
{
	DWORD LocalID;	//发送方地址
	DWORD DestAddress;	//接收方地址	
	BYTE  GraphID;	//电文标识号
	BYTE  TotalPackNum;	//电文总包数
	BYTE  CurPackNum;	//当前分包顺序号
	DWORD InfoLen;	//电文长度(单位：字节)
	BYTE  InfoBuff[420+1];//汉字:Max 120个  BCD:Max 420
	BOOL  ifSecret;	//固定填零
	BOOL  ifQuick;	//00:普通  01:特快
	BOOL  ifBCD;	//0:汉字  1:代码
}GraphSplitReq, *lpGraphSplitReq;

//=========================//
//======用户机-->外设=======//
//=========================//


//功率状况
//CUNPACK_GLZK :1
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

//北斗定位信息
//CUNPACK_DWXX
typedef struct  tagPosInfo
{
	DWORD LocalID;	//本机ID(接收方ID，含子用户)
	BOOL  InfoType;	//0:定位  1:查询
	BOOL  ifSecret;	//0:无密钥  1:有密钥
	BOOL  Precision;	//0:一档(20m)  1:二档(100m)
	BOOL  ifUrgent;		//0:否  1:紧急
	BOOL  ifMultiValue;	//0:否  1:多值解 (表示本帧传输的定位信息是否为正确的定位信息)
	BOOL  ifHighAlt;	//[标准协议2.1版无此项] 高程指示[0:普通  1:高空]
	DWORD QueryAddress;	//指挥型用户所查询用户的用户地址
	SYSTEMTIME PosTime;	//定位时间
	BYTE  LongDeg;	//经度－度
	BYTE  LongMin;	//分
	BYTE  LongSec;	//秒
	BYTE  LongDecsec;	//0.1秒
	BYTE  LatDeg;	//纬度－度
	BYTE  LatMin;	//分
	BYTE  LatSec;	//秒
	BYTE  LatDecsec;	//0.1秒
	short Altitude;	//高程
	short Ef;		//高程异常
}PosInfo, *lpPosInfo;

//通信信息
//CUNPACK_TXXX
typedef struct tagCommInfo
{
	DWORD LocalID;	//本机ID(接收方ID，含子用户)
	BOOL  ifBCD;	//0:汉字  1:代码
	BOOL  InfoType;	//0:通信  1:查询
	BOOL  ifSecret;	//0:无密钥 1:有密钥
	DWORD SrcAddress;	//发信方地址
	BYTE  CommHour;	//通信信息时此项为0
	BYTE  CommMin;	//通信信息时此项为0
	DWORD CommLen;	//电文长度(单位：bit)  不能传字节数，因为BCD可能不是整字节
	BYTE  CommBuff[420+1];
	BOOL  CRCFlag;	//00:CRC校验正确 01:CRC校验错误
}CommInfo, *lpCommInfo;

//通信回执
//CUNPACK_TXHZ
typedef struct tagCommAck
{
	DWORD LocalID;	//本机ID
	DWORD DestAddress;	//收信方地址
	BYTE  AckCount;	//回执数
	BYTE  SendGraphHour[5];	//发信时间
	BYTE  SendGraphMin[5];
	BYTE  AckHour[5];	//回信时间
	BYTE  AckMin[5];

	BOOL bFailed;
	BYTE byFailedCode;
	BYTE byFailedTime[6];

}CommAck, *lpCommAck;

//口令识别
//CUNPACK_KLSB
typedef struct tagPswIdentify
{
	DWORD LocalID;	//本机ID(接收方ID，含子用户)
	BOOL  ifBCD;	//0:汉字  1:代码
	BOOL  ifSecret;	//0:无密钥 1:有密钥
	BYTE  PswAck;	//口令应答
	DWORD CommLen;	//电文长度(单位：bit)  不能传字节数，因为BCD可能不是整字节
	BYTE  CommBuff[21+1];
}PswIdentify, *lpPswIdentify;

//定时结果[双向授时]
//CUNPACK_DSJG
typedef struct tagTimeRes
{
	DWORD LocalID;
	SYSTEMTIME  ExactTime;
	BYTE  BeamNo;	//[二次协议2.1版无此项] 当前波束号[001～110:1～6波束]
	BYTE  BeamPower;	//[二次协议2.1版无此项] 信号功率
	BOOL  LocalStatus;	//[二次协议2.1版无此项] 本机状态[00:正常  01:异常]
	BOOL  ifBeamLock;	//[二次协议2.1版无此项] 卫星锁定[00:锁定  01:失锁]
	BOOL  SatelliteStatus;	//[二次协议2.1版无此项] 卫星状态[00:正常  01:异常]
	BOOL  CRCFlag;	//[二次协议2.1版无此项] CRC校验[00:正确  01:错误]
	BOOL  TimeSys;	//[二次协议2.1版无此项] 类型[00:北斗时  01:UTC时]
	DWORD UniZero;	//[二次协议2.1版无此项] 单向零值
	BYTE  LongDeg;		//经度－度
	BYTE  LongMin;		//分
	BYTE  LongSec;		//秒
	BYTE  LongDecsec;	//0.1秒
	BYTE  LatDeg;		//纬度－度
	BYTE  LatMin;		//分
	BYTE  LatSec;		//秒
	BYTE  LatDecsec;	//0.1秒
	int	  Altitude;
	int   Revise;	//[二次协议2.1版无此项] 修正值[有符号，单位:1纳秒]
}TimeRes, *lpTimeRes;

//时差数据
//CUNPACK_SCSJ
typedef struct tagTimeDiffData
{
	DWORD LocalID;
	int   TimeDiffValue;	//[有符号，单位:1纳秒]
}TimeDiffData, *lpTimeDiffData;

//当前工况
typedef struct tagCurStatus
{
	DWORD	LocalID;
	BYTE	CheckType;	//00:自检  01:巡检
	BOOL	Status;		//00:正常  01:异常
}CurStatus, *lpCurStatus;

//广播输出[二次协议3.0版]
//CUNPACK_GBSC
typedef struct tagBroadRes
{
	DWORD LocalID;
	DWORD Time;	    //北斗系统时刻[单位：1分钟]
	BYTE  LeapSec;	//闰秒[单位：1秒]
	int   TimeDiffer;	//时差[有符号，单位：10纳秒]
	BYTE  AntennaNum;	//天线号
	int   SatlX;	//卫星位置参数[有符号，单位：1米]
	int   SatlY;
	int   SatlZ;
	short SpeedX;	//卫星速度参数[有符号，单位：0.01米/秒]
	short SpeedY;
	short SpeedZ;
	DWORD TimeDelay;	//时延[单位：1纳秒]
	short a[17];	//电波传播修正模型参数[有符号，单位：1纳秒]
	BYTE  YearCount;	//年计数[2000年计为0]
	BYTE  LeapSecStatus;	//闰秒状态[00:无闰秒  01:正闰秒  02:负闰秒]
	BYTE  BeamStatus;	//波束状态[1101:东星  1110:西星  1111:备份星  0000:本波束故障]
	BYTE  SysStatus[5];	//系统状态指示(待定)
}BroadRes, *lpBroadRes;

//公用输出
//CUNPACK_GYSC
typedef struct tagCommonRes
{
	DWORD LocalID;
	BYTE  Len;
	BYTE  CommonInfo[48];	//公用信息(384bit)
}CommonRes, *lpCommonRes;

//I路信息
//CUNPACK_ILXX
typedef struct  tagIFrameInfo
{
	BYTE BeamNo;	//波束号
	BYTE IDataSeg[30];	//I路数据段(221bit) [二次协议2.1版]含分帧号、公用、广播、抑制、ID1、ID2、I路信息类别、I路数据段项
}IFrameInfo, *lpIFrameInfo;

//CUNPACK_BSXX
typedef struct  tagBSXXInfo
{
	int nUserID;
	int nBeamSend;
	int nBeamTimeDiff;
}BSXXInfo, *lpBSXXInfo;

//CUNPACK_RNCS
typedef struct  tagRNCSInfo
{
	int nUserID;
	BYTE byWeek;
	BYTE byParams[256];
}RNCSInfo, *lpRNCSInfo;

//Q路信息
//CUNPACK_QLXX
typedef struct  tagQFrameInfo
{
	BYTE BeamNo;	//波束号
	BYTE QDataSeg[30];	//Q路数据段(228bit) [二次协议2.1版]含Q路信息类别、Q路数据段项
}QFrameInfo, *lpQFrameInfo;

//读卡信息
//CUNPACK_ICXX
typedef struct tagCardInfoRead
{
	DWORD LocalID ;
	BYTE FrameNum ;				//帪号
	//第0帧数据用到的变量
	DWORD BroadcastID ;			//通播ID
	BYTE MachineCharacter ;		//用户机特征
	WORD ServeFreq ;			//服务频度
	WORD CommLen;
	BYTE CommuLeave ;			//通信级别
	BYTE EncryptFlag ;			//加密标志
	WORD SubordinateNum ;		//下级用户总数
	//第N帧数据用到的变量
	BYTE CurrFrameSubNum;		//本帧下属用户数
	DWORD lpSubUserAddrs[100] ;	//指向下级用户地址的指针
} CardInfoRead,*lpCardInfoRead;

//自毁确认
//CUNPACK_ZHQR
typedef struct  tagConfirmSelfDes
{
	DWORD LocalID ;
	DWORD DesFlag ;
}ConfirmSelfDes,*lpConfirmSelfDes;


//自检信息
//CUNPACK_ZJXX
typedef struct tagSelfCheckInfo
{
	DWORD LocalID ;
	BYTE ICCardState ;		// IC卡状态
	BYTE HardState ;		// 硬件状态
	BYTE Batterystate ;		// 电池电量
	BYTE InState ;			// 入站状态
	BYTE PowerState[10] ;	// 功率状况
}SelfCheckInfo,*lpSelfCheckInfo;

//零值信息
//CUNPACK_LZXX
typedef struct tagZeroInfo
{
	DWORD LocalID ;
	DWORD ZeroValue ;
}ZeroInfo,*lpZeroInfo;

//时钟信息
//CUNPACK_SJXX
typedef struct tagSClockInfo
{
	DWORD LocalID;
	WORD Year ;
	BYTE Month ;
	BYTE Day ;
	BYTE Hour ;
	BYTE Minite ;
	BYTE Second ;
}ClockInfo,*lpClockInfo;

//版本信息
//CUNPACK_BBXX
typedef struct tagEditionInfo
{
	DWORD LocalID ;
	int Num ;
	BYTE info[492] ;  //版本信息，以逗号分割，可以自定义意义
}EditionInfo,*lpEditionInfo;


//序号信息
//CUNPACK_XHXX
typedef struct tagSerialInfo
{
	DWORD LocalID ;
	DWORD SerialNum ;
	BYTE ManageInfo[65] ;
}SerialNum,*lpSerialNum;

//反馈信息
//CUNPACK_FKXX
typedef struct tagFeedbackInfo
{
	DWORD LocalID ;
	BYTE FeedResult; //反馈结果
	char Reserve[4] ;   // 保留
}FeedbackInfo,*lpFeedbackInfo;


//GPS定位信息
typedef struct tagGpsInfo
{
	DWORD LocalID;
	SYSTEMTIME PosTime;
	BYTE  LongDeg;	//经度－度
	BYTE  LongMin;	//分
	BYTE  LongSec;	//秒
	BYTE  LongDecsec;	//0.1秒
	BYTE  LatDeg;	//纬度－度
	BYTE  LatMin;	//分
	BYTE  LatSec;	//秒
	BYTE  LatDecsec;	//0.1秒
	short Altitude;	//高度(单位：米)
	WORD  Speed;	//速度(单位：0.01m/s)
	WORD  Direction;//方向(单位：0.01度)
	BYTE  Status;	//状态
}GpsInfo, *lpGpsInfo;

//======扩展协议=======//

//链路维护响应
typedef struct tagLoopAck
{
	BYTE  SequenceNum;//接收顺序号
}LoopAck, *lpLoopAck;

//告警信息
typedef struct tagAlarmInfo
{
	BYTE  AlarmCode;	//告警码: F0H:电源告警, F1H：DSP通路故障, F2H：DSP损坏, F3H：信号失锁
}AlarmInfo, *lpAlarmInfo;

//系统抑制
typedef struct tagSysRestrain
{
	BYTE  RestrainFlag;//00:结束系统抑制 01:系统抑制
}SysRestrain, *lpSysRestrain;

//电源检测指示
typedef struct tagPowerInfo
{
	BYTE	ElecCapacity;	//电量指示0-5六档指示
}PowerInfo, *lpPowerInfo;

//发射指示
typedef struct tagEmitIndication
{
	BYTE  IndicateCode;		//发射指示码
}EmitIndication, *lpEmitIndication;

//校时信息(单向授时)
typedef struct tagTellTimeInfo
{
	WORD  wDay;
	BYTE  bHour;
	BYTE  bMinute;
	BYTE  bSecond;
	BYTE  bCentiSec;
}TellTimeInfo, *lpTellTimeInfo;

//DEVICE_BDBSS
typedef struct tagBSSZ 
{
	int beamNo  ;//响应波束
	int beamDiff;//时差波束
}BSSZ_STRUCT,*pBSSZ_STRUCT;

//DEVICE_BDJMS
typedef struct tagBDJMS_STRUCT
{
	BOOL bInhabit;//是否静默
}BDJMS_STRUCT,*pBDJMS_STRUCT;

//DEVICE_BDICA...
typedef struct tagBDICA_STRUCT
{   
	BYTE byType;//0:检测本机加解密模块信息。1:检测具备指挥功能用户设备的下属用户信息。
	int  nSubUsrFrameNo;//当指令类型等于0时，下属用户信息帧号填0.
}BDICA_STRUCT,*pBDICA_STRUCT;
typedef BDICA_STRUCT ICAInfo;

//管信注入指令 ： DEVICE_BDGXM
typedef struct tagGXZR 
{
	BYTE byType;
	BYTE manageInfo[65];
	UINT uSerialNumber;
}GXZR_STRUCT,*pGXZR_STRUCT;

typedef struct tagDWSQ_STRUCT
{
	BOOL bifUrgency;
	BYTE bPosType;//0:有高程1：无高程，2：测高1，3测高2。
	BOOL bifHigh;
    
	BOOL ifSecret;
	BOOL ifHighAlt;
    
	int userAddr;
	double fAlti;
	double fTianXian;
	double fQiYa;
	double fWenDu;
	DWORD dwFreq;
}DWSQ_STRUCT,*pDWSQ_STRUCT;

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

}BDDWA_STRUCT,*pBDDWA_STRUCT;

typedef BDDWA_STRUCT   DWAInfo;
typedef pBDDWA_STRUCT  lpDWAInfo;

//定时申请 DEVICE_BDDSA
typedef struct tagDSSQ_STRUCT
{
	BOOL bifHavePos;//TRUE:有概略位置；FALSE无概略位置，此时经纬度为空。
	BYTE bPosInfo;//no use...
	int  nType;//单向1;双向2.
	
	int userAddr;
	double fL;
	double fB;
	double fH;
	int iUniZero;//单向零值，当双向定时申请时，该参数填0；
	int iAddZero;//指外加电缆等对授时造成的附件零值。
	BYTE bOutSet;//no use...
	DWORD dwFreq;
}DSSQ_STRUCT,*pDSSQ_STRUCT;

//通信查询 DEVICE_BDCXA
typedef struct tagBDCXA_STRUCT
{   
	BYTE byQueryCat;//0:定位查询;1通信查询
	BYTE byQueryType;//查询类别为0时，1：一次定位查询；2：二次定位查询；3：三次定位查询
					 //查询类别为1时，1：最新存入电文查询，用户地址为空；2：按发信地址查询，用户地址为发信方地址；3：回执查询，用户地址为收信方地址
	int  nUserId;
}BDCXA_STRUCT,*lpBDCXA_STRUCT;

//数据输出控制指令 DEVICE_RMO
typedef struct tagRMOInfo
{   
	char chName[4];//合法的标识串，如"GGA"
	BYTE byMode;   //模式。1:关闭指定语句，2:打开指定语句，3:关闭全部语句，4:打开全部语句。byMode == 3或byMode ==4时:目标语句数据保留区为空。
	int  nOutFreq; //单位:秒。byMode == 4时,此保留区域为空。
}RMOInfo,*lpRMOInfo;

typedef struct tagSubUserPos
{
	int  nUsrId;
	BYTE byPrecisionInd;//0:一档 1:二档
	BOOL bUrgent;
	BOOL bMutiVal;
	BOOL bGaoKong;//0:普通用户；1:高空用户。

	BYTE  byPosHour;//定位时刻
	BYTE  byPosMin;
	float fPosSec;
	
	double dfLat;//纬度：单位°
	double dfLong;//经度：单位°;
	float  fHgt;//高程，单位：米
	float  fHgtXcept;//高程异常，单位：米
}SubUserPos ,*LPSubUserPos;

typedef struct tagSubUserMsg
{   
	int  nRcvrId;
	int  nSendId;
	BYTE byCodeType;//0 : 汉字；1:代码，2:混发
	int  nSendHour;
	int  nSendMin;
	int  nCommLen;
	char chContent[512];
	int  nCommBitsLen;
	BYTE byCrcFlag;
}SubUserMsg ,*LPSubUserMsg;

typedef struct tagRNSAck
{   
	int nPackNext;
	int nReserved1;
	int nReserved2;
	int nReserved3;

}RNSAck ,*LPRNSAck;


typedef struct tagRNSCmd
{   
	int   nTotalPack;
	int   nCurPack;
	int   nPackLen;
	char  chContent[257];  
	
}RNSCmd ,*LPRNSCmd;

typedef struct tagAPLCmd
{
	//utc时间
	int     nHour;
	int     nMinute;
	float   fSecond;	
	float	LevelH;			// 水平保护门限
	float	LevelV;			// 垂直保护门限
	float	Level3D;		// 空间保护门限	
}APLCmd,*LPAPLCmd;

typedef tagAPLCmd  tagAPLInfo;
typedef tagAPLCmd  APLInfo;
typedef tagAPLCmd *LPAPLInfo;


//nMode == 0 : 1pps
//nMode == 1 : B码
typedef struct SecondPulseMode
{
	int nMode;
}PulseMode,*LPPulseMode;

// 定位模式（GET_BID）
typedef struct tagBIDInfo
{   	
	UINT8 nFrec; // 1=B1,2=B1B3,3=B3,4=B3L1G1,5=L1,6=G1,7=B1L1
	char  cCP;	 // 'C'=C码，'P'=P码，'A'=P码C码
} BIDInfo,*LPBIDInfo;


typedef struct tagZHRInfo
{	
	char ZHInfo[16]; // "AA5555AA"表明自毁成功
} ZHRInfo,*LPZHRInfo;

//一代扩展协议结构体，态势报告
typedef struct UserPosition
{
	DWORD UserID;		//用户地址
	BYTE Day;			//定位时刻的日数据
	BYTE Hour;			//定位时刻的小时数据
	BYTE Min;			//定位时刻的分数据
	BYTE Second;		//定位时刻的秒数据
	BYTE DecSec;		//定位时刻的.01秒数据
	BYTE LongDeg;		//经度
	BYTE LongMin;		//经分
	BYTE LongSec;		//经秒
	BYTE LongDecsec;	//0.1秒为单位
	BYTE LatDeg;		//纬度
	BYTE LatMin;		//纬分
	BYTE LatSec;		//纬秒
	BYTE LatDecsec;		//0.1秒为单位
}UserPosition,*lpUserPosition;

struct PosData
{
	UINT32 UserID;			// 用户ID
	UINT32 T;				// 时间，秒(日+时+分+秒)
	float L;				// 经度，度
	float B;				// 纬度，度
} ;

typedef struct tagUserPositionInfo
{
	DWORD DiffIDNum ;
	DWORD DiffTimeNum ;
	DWORD DiffLDegreeNum ;
	DWORD DiffBDegreeNum ;
}UserPositionInfo,*lpUserPositionInfo;

typedef struct tagShitaiData
{   
	DWORD LocalID;
	DWORD DestAddress;

	DWORD StandID ;
	BYTE  IDDiffBitNum ;
	DWORD StandTime ;
	BYTE  TimeDiffBitNum ;
	DWORD StandLDegree ;
	BYTE DiffLDegreeBitNum;
	DWORD StandBDegree ;
	BYTE DiffBDegreeBitNum;
	int IDNum ;
	UserPositionInfo lpinfo[500] ;
}ShitaiData,*lpShitaiData;

//态势数据接收
typedef struct tagShitaiDataRecv
{
	DWORD LocalID;
	DWORD SrcAddress;
	
	DWORD StandID ;
	BYTE  IDDiffBitNum ;
	SYSTEMTIME StandTime ;
	BYTE  TimeDiffBitNum ;
	DWORD StandLDegree ;
	BYTE DiffLDegreeBitNum;
	DWORD StandBDegree ;
	BYTE DiffBDegreeBitNum;
	
	int UserNum ;
	lpUserPositionInfo lpinfo;

}ShitaiDataRecv,*lpShitaiDataRecv;

#endif
