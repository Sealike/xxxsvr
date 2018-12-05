#ifndef _BDJOBBER_CALLBACK__H__
#define _BDJOBBER_CALLBACK__H__

#ifdef __cplusplus
extern "C"
{
#endif
void  bdjobber_exp_cb(int msgid ,void* msg,int len,void* param);
int Seqno2FrontsvrManage(void);
#ifdef __cplusplus
}
#endif

//#include "minwindef.h"

//typedef struct _jobber_free_st
//{
//	pthread_mutex_t mt;
//	BOOL free;
//}jobber_free_st, *jobber_free_t;



////波束状态..
//typedef struct tagBeamInfo
//{
//	BYTE  byResponseNo;
//	BYTE  byTimeDiffNo;
//	BYTE  byPower[10];		// 波束信号的强度
//}BeamInfo, *lpBeamInfo;

//波束状态..
typedef struct tagBeamInfo
{
	char  byPower[30];		// 波束信号的强度
	unsigned char  byResponseNo;
	unsigned char  byTimeDiffNo;
	//unsigned char  byPower[10];		// 波束信号的强度
	
}BeamInfo, *lpBeamInfo;

////位置报告1双向语句 ：DEVICE_WAA
//typedef struct tagWAAInfo
//{
//	int    nDestionID; //接收方ID（新增）GJ 2016.3.29
//	BYTE   byInfoType;//信息类型。0-表示为用户设备接收的位置报告信息，此时nReportFreq无效。nUserId为发送位置报告的用户的地址。
//	//1-表示为用户设备发送的位置报告信息，此时nUserId为接收位置报告信息的用户地址。
//	int    nReportFreq; //报告频度，单位:秒。
//	int    nUserId;  //发信方ID
//	WORD   wReportHour;//位置报告时间信息
//	WORD   wReportMinute;
//	float  fReportSecond;
//	double dfLat;//纬度，单位:度。
//	char   chLatDir;//'N'/'S';
//	double dfLong;//经度，单位:度。
//	char   chLongDir;//'E'/'W';
//	double dfHgt;//高程
//	char   chHgtUnit;//高程单位:m
//}WAAInfo, *lpWAAInfo;

typedef enum
{
	warn_low_power = 0,
	warn_destroy,
	req_unlock,
	req_taskpau,
	req_taskcon,
	unknown
}bd_evt_type_t;


//位置报告1双向语句 ：DEVICE_WAA
typedef struct tagWAAInfo
{
	int    nDestionID; //接收方ID（新增）GJ 2016.3.29
	unsigned char   byInfoType;//信息类型。0-表示为用户设备接收的位置报告信息，此时nReportFreq无效。nUserId为发送位置报告的用户的地址。
	//1-表示为用户设备发送的位置报告信息，此时nUserId为接收位置报告信息的用户地址。
	int    nReportFreq; //报告频度，单位:秒。
	int    nUserId;  //发信方ID
	unsigned char   wReportHour;//位置报告时间信息
	unsigned char   wReportMinute;
	float  fReportSecond;
	double dfLat;//纬度，单位:度。
	char   chLatDir;//'N'/'S';
	double dfLong;//经度，单位:度。
	char   chLongDir;//'E'/'W';
	double dfHgt;//高程
	char   chHgtUnit;//高程单位:m
}WAAInfo, *lpWAAInfo;

//通信信息常量定义 : CUNPACK_TXR 
typedef struct tagTXRInfo
{
	unsigned char byInfoType;//信息类别:1~5。1:普通通信，2:特快通信，3:通播通信,4:按最新存入电文查询获得的通信,5:按发信方地址查询获得的通信。
	int  nLocalID;		//发信方地址
	int  nDestionID;	//收信方地址
	unsigned char byTeleFormat;//电文形式:0-汉字，1-代码，2-混合传输
	unsigned char bySendHour;  //发信时间(时)。byInfoType == 1，2，3时，发信时间无效。byInfoType ==4，5时,为被查询的通信电文在中心控制系统记录的发送时间。
	unsigned char bySendMinute;//发信时间(分)。
	char chContent[1024];//通信内容。C串。当byTeleFormat == 2时，电文内容首字母固定为A4.
	int  nContentLen;
	unsigned char byCRC; //CRC校验，0-正确 1-错误

	bd_evt_type_t EventType;	// 事件类型。   1：低电量告警 2：破坏告警	3：开锁请求	4：任务暂停请求	5: 任务恢复请求
	char  time[15];
	char  digest[33];
	double dfLong;
	double dfLat;
	float  fEarthHgt;			// 大地高.
}TXRInfo, *lpTXRInfo;

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
	unsigned char   byPrecisionInd; //定位精度指示.0:一档20米精度;1:二档100米精度
	int   bUrgent;        //是否紧急
	int   bMultiValue;    //是否多值解
	int   bGaoKong;       //高空指示

}DWRInfo, *lpDWRInfo;


#endif