#ifndef  __CONSTDEFINE_H
#define  __CONSTDEFINE_H
//////////////////////////////////////////////////////////////////////////
//此文件包含所有常量定义，所作改动必须同步更改DllInterface.h
//////////////////////////////////////////////////////////////////////////
#define PROTOCAL_EX_API     extern "C" __declspec(dllexport)
#define PROTOCAL_IM_API     extern "C" __declspec(dllimport)
#define PROTOCAL_EX_CLASS              __declspec(dllexport)
#define PROTOCAL_IM_CLASS              __declspec(dllimport)
#define DLLMAKEMFCCALL()    AFX_MANAGE_STATE(AfxGetStaticModuleState());


enum{
	CUNPACK_NULL,
	CUNPACK_BSI,//波束信息1
	CUNPACK_DWR,//定位信息2
	CUNPACK_FKI,//反馈信息3
	CUNPACK_ICI,//加解密模块信息4
	CUNPACK_ICZ,//指挥机下属用户信息5
	CUNPACK_KLS,//口令识别应答6
	CUNPACK_HZR,//回执信息7
	CUNPACK_TXR,//通信信息8
	CUNPACK_SBX,//设备相关参数9
	CUNPACK_ZTI,//用户设备状态信息10
	CUNPACK_ZDA, //UTC时间，日期，和本地时区等信息11
	CUNPACK_LZM,//设备零值信息12
	CUNPACK_WAA,//位置报告13
	CUNPACK_WBA,//位置报告14
	CUNPACK_BGJG,//15未用。
	CUNPACK_GGA,//16RNSS定位结果
	CUNPACK_RMC,//17RNSS定位测速信息
	CUNPACK_GSV,//18可见卫星信息
	CUNPACK_PMU,//19PMU输出语句，PRM时效参数信息。
	CUNPACK_ICM,//20
	CUNPACK_PKY,//21PRM工作参数动态加注。
	CUNPACK_ALM,//22双向语句，描述卫星历数。
	CUNPACK_BSD,//23标示点信息
	CUNPACK_BSQ,//24标示区信息
	CUNPACK_BSX,//25标示线信息
	CUNPACK_ZTG,
	CUNPACK_GBS,//GNSS卫星故障检测。
	CUNPACK_GLL,
	CUNPACK_ECT,
	CUNPACK_KLT,
	CUNPACK_GSA,//参与定位卫星
	CUNPACK_GXM,
	CUNPACK_GLZK,		//功率状况 01
	CUNPACK_DWXX,		//定位信息 02
	CUNPACK_TXXX,		//通信信息 03
	CUNPACK_TXHZ,		//通信回执 04
	CUNPACK_KLSB,		//口令识别 05
	CUNPACK_DHSC,		//导航输出 06
	CUNPACK_DSJG,		//定时结果 07
	CUNPACK_SCSJ,		//时差数据 08 
	CUNPACK_ZBSC,		//坐标输出 09
	CUNPACK_GBSC,		//广播输出 10
	CUNPACK_GYSC,		//公用输出 11 
	CUNPACK_ICXX,		//IC信息   12
	CUNPACK_ZHQR,		//自毁确认 13
	CUNPACK_ZJXX,		//自检信息 14
	CUNPACK_LZXX,		//零值信息 15
	CUNPACK_SJXX,		//时间信息 16
	CUNPACK_BBXX,		//版本信息 17 
	CUNPACK_XHXX,		//序号信息 18
	CUNPACK_GLXX,		//管理信息 19
	CUNPACK_FKXX,		//反馈信息 20
	CUNPACK_BSXX,		//标识信息 21
	CUNPACK_HXXX,		//航线信息 22
	CUNPACK_HJSC,		//航迹输出 23
	CUNPACK_ILXX,		//I路信息  24 
	CUNPACK_QLXX,		//Q路信息  25
	CUNPACK_RNCS,		//Q路信息  26
	CUNPACK_QUERYPOS,	//定位查询(从定位信息中解出)
	CUNPACK_QUERYCOMM,	//通信查询(从通信信息中解出)
	//------------通信协议扩展
	CUNPACK_MIXCOMMINFO,//混发电文通信信息(协议0xA0)
	CUNPACK_POSREPORT,	//位置报告接收(协议0xA4)
	CUNPACK_XSD,//表示下属定位信息
	CUNPACK_XST,//表示下属通信信息
	CUNPACK_DHV,//表示速度信息
	CUNPACK_RNS,
	CUNPACK_RPQ,
	CUNPACK_TSQ,
	CUNPACK_WZB,
	CUNPACK_ZYB,
	CUNPACK_APL,
	CUNPACK_BID,
	CUNPACK_ZHR,

	CUNPACK_CLOSE=255,
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
	 DEVICE_BDBSS =23,	  //RDSS 波束设置指令2.1
	 DEVICE_BDCXA =24,    //RDSS 查询指令2.1
	 DEVICE_BDRMO =25,
	 DEVICE_BDPKY =26,
	 DEVICE_BDGXM =27,		//RDSS 管信注入指令
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
	 DEVICE_BDZHS =46,//自毁申请
	 DEVICE_BDECS =47,
	 DEVICE_SATDEL=48,
	 DEVICE_MODPPS=49,
	 DEVICE_BDRNS =50,
	 DEVICE_BDRPQ =51,//位置报告请求。
	 DEVICE_BDAPL =52,
	 DEVICE_SECPULSE = 53,
};

//typedef enum _USER_TYPE{
//	LOCALUSER=1,
//	SUBUSER,
//	OTHERUSER,
//}user_type;

#define MAXCHILDRENNUM	50
/* IQ支路误码测试数据流长度 */
#define IFRAMEDATALEN	221
#define QFRAMEDATALEN	228 

#define MASK_CHARGE       0x01
#define MASK_POWER_INOUT  0x02
#define MASK_CHANNEL      0x04
#define MASK_ANTENNA      0x08
#define MASK_ENCRYPMODULE 0x10
#define MASK_PRMMODULE    0x20

#define PROT_BD_2_1       "proto_bd_2_1"

#endif
