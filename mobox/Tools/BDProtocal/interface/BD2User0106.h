#if !defined(BD2_USER_H)
#define BD2_USER_H

#include "nscHead.h"


#define BD2_UFUN_USER_START 90000	// 功能起始号

// 数据源类型
enum STYPE
{
	STYPE_BD,	//北斗二号导航定位系统(BD-2)
	STYPE_GP,	//全球定位系统(GPS)
	STYPE_GN,	//全球导航卫星系统(GNSS)
	STYPE_GL,	//GLONASS系统
	STYPE_GA,	//伽利略系统
	STYPE_CC,	//计算机系统
};

#define __SUCCESS  1
#define __FAILED   0

// 日期：D[3]=年(YY)-月(MM)-日(DD)
// 时间：T[4]=时-分-秒-1/100秒
// 经度: L[4]=度-分-秒-1/10秒
// 纬度: B[4]=度-分-秒-1/10秒
// GGA中特别定义,因为要精确到0.0001秒
// 经度: L[5]=度-分-秒-1/10秒-1/1000秒
// 纬度: B[5]=度-分-秒-1/10秒-1/1000秒

//----------------------------------------------------------------------------------------------
// 发送命令常数
//----------------------------------------------------------------------------------------------

enum BD2_COMM_UFUN_SET
{	
	UFUN_SET_RMO=BD2_UFUN_USER_START,		// 请求输出
	UFUN_SET_APL,							// 完好性保护门限
	UFUN_SET_MSS,							// 模式设置
	
	UFUN_SET_DWA,							// 定位申请,返回:UFUN_GET_DWR
	UFUN_SET_TXA,							// 通信申请,返回:UFUN_GET_TXR
	UFUN_SET_CXA,							// 查询申请,返回:UFUN_GET_DWR,UFUN_GET_TXR,UFUN_GET_HZR
	
	UFUN_SET_WAA,							// 位置报告1(电文方式)
	UFUN_SET_WBA,							// 位置报告2(定位方式)
	
	UFUN_SET_KLS,							// 口令识别命令(指挥机对下属)
	UFUN_SET_KLT,							// 口令识别应答(下属对指挥机)

	UFUN_SET_ICA,							// 用户卡检测,返回:UFUN_GET_ICZ,UFUN_GET_ICI
	UFUN_SET_ZHS,							// 紧急自毁
	UFUN_SET_JMS,							// 静默设置

	UFUN_SET_GXM,							// 管信设置/读取
	UFUN_SET_BSS,							// 波束设置

	UFUN_SET_RPQ,                           // 位置报告请求
	UFUN_SET_TSQ,				            // 态势请求
	UFUN_SET_FTX,                           // 自由通信
	UFUN_SET_WZB,                           // 位置报告
	UFUN_SET_SEC                            //1pps秒输出控制
};


#define WZBG_TYPE_BJZ54		0xA0
#define WZBG_TYPE_WGS84		0xA1
#define WZBG_TYPE_CGS2K		WZBG_TYPE_WGS84

#pragma pack(push)
#pragma pack(4)
//----------------------------------------------------------------------------------------------
// 发送命令的参数结构
//----------------------------------------------------------------------------------------------
//一代扩展协议结构体,位置报告。

//位置报告数据A0
//750...
typedef struct tagLSET_WZBG
{
	UINT32   srcID;         // 本机用户地址
	UINT32   PeerID;        // 对方地址
	UINT8    PosType;       // 定位坐标系类型(WZBG_TYPE_xxxx)
	UINT8    T[4];			// 定位时间(时/分/秒/百分之一秒)
	UINT8	 L[4];			// 定位经度(度/分/秒/十分之一秒)
	UINT8    B[4];			// 定位纬度(度/分/秒/十分之一秒)
	int		 H;				// 高程(单位米)
	int      Segma;			// 高程异常(单位米)
	// 缺高程指示？默认为普通
}LSET_WZBG;

typedef LSET_WZBG  USET_WZBG;
typedef LSET_WZBG  *lpUSET_WZBG;
typedef LSET_WZBG  UGET_WZBG;
typedef LSET_WZBG  *lpUGET_WZBG;
typedef  USET_WZBG  UGET_WZBG;
//一代扩展协议结构体,自由报文。
// 自由电文A4
//750..
typedef struct tagLSET_ZYDW 
{
	UINT32   srcID;			// 本机用户ID
	UINT32	 PeerID;		// 对方地址
	UINT8	 nLength;		// 正文长度，字节
	char	 Msg[210];		// 正文
}LSET_ZYDW;

typedef LSET_ZYDW  USET_ZYBW;
typedef LSET_ZYDW  USET_ZYDW;
typedef LSET_ZYDW  UGET_ZYBW;
typedef LSET_ZYDW  UGET_ZYDW;

//一代扩展协议结构体,位置报告请求。。
//要求位置报告A2
 //750...
typedef struct tagLSET_YQBG
{   
	UINT32   srcID;			// 受控用户ID,一般没用,填零
	UINT32   PeerID;		// 收方
	UINT8    Freq;			// 频度(秒)
	UINT8    Flag;			// 例外标志（0响应/1不响应）	
	UINT8	 SubCount;		// 下属用户列表中的个数
	UINT32	 SubID[22];		// 下属用户列表
}LSET_YQBG;

typedef LSET_YQBG  USET_RPQ;
typedef LSET_YQBG  *lpUSET_RPQ;
typedef LSET_YQBG  UGET_RPQ;
typedef LSET_YQBG  *lpUGET_RPQ;

typedef USET_RPQ  UGET_RPQ;

 //态势数据A3
typedef struct tagLSET_TSSJ
{
	UINT32   srcID;				// ID
	UINT32	 PeerID;			// 对方地址
	UINT8	 UserCount;
	struct 
	{
		UINT32 UserID;			// 用户ID
		UINT32 T;				// 时间，秒(日+时+分+秒)
		float L;				// 经度，度
		float B;				// 纬度，度
	} PosData[60];//此内容中不能出现相同的ID号；确保填入的数据经过组包后不会超过通信容量。
} LSET_TSSJ;
typedef LSET_TSSJ USET_TSQ;
typedef LSET_TSSJ *lpUSET_TSQ;
typedef LSET_TSSJ UGET_TSQ;
typedef LSET_TSSJ *lpUGET_TSQ;


// 请求输出
typedef struct tagUSET_QQSC
{   
	tagUSET_QQSC(){memset(this, 0, sizeof(tagUSET_QQSC));};
	STYPE	src;            //
	UINT8	Mode;			// 模式，1=关闭指定语句，2=打开指定语句，3=全关，4=全开
	UINT16	Freq;			// 频度(单位ms)（模式2时使用）
	char	Cmd[4];			// 语句名（3个字符,下面接收消息结构UGET_XXX中的XXX）
}USET_QQSC,USET_RMO;

// 定位申请
// 只有能接收到两个波束的信号(这两个波束必须分别来自不同的卫星)才能进行定位.
// 测高方式影响定位的结果和精度

//当“测高方式”为:
//    有测高: 大地高:为天线处的大地高程,单位米; 
//    无测高: 天线高:为天线距离地面的高度，单位为0.1米；
//    测高1:  天线高:为天线距离地面的高度，单位为0.1米；
//			  气压:  天线处的大气压,单位0.1Hp,如果用户机内部自带气压仪，该数据填“0”;
//			  温度:  当时的气温,单位0.1℃,如果用户机内部自带温度计，该数据填“0”;
//    测高2:  大地高,为用户机中气压仪所处位置的正常高,单位米，
//            天线高:天线距离用户机中气压仪的高度，单位为0.1米
//			  气压:  天线处的大气压,单位0.1Hp,如果用户机内部自带气压仪，该数据填“0”;
//			  温度:  当时的气温,单位0.1℃;

typedef struct tagUSET_DWSQ
{
	tagUSET_DWSQ(){memset(this, 0, sizeof(tagUSET_DWSQ));};
	UINT32 UserID;				// 定位的用户机ID
	UINT8  Flag;				// 信息标志,参见下面的掩码
	UINT16 Freq;				// 入站频率(单位秒),频度为0时,只定位一次
	INT32  Altitude;			// 大地高（单位米）
	UINT32 AntHeight;			// 天线相对地面的高（单位0.1米）
	UINT32 Pressure;			// 气压（单位0.1Hp）
	INT16  Temperature;			// 温度（单位0.1度）
} USET_DWSQ,USET_DWA;

#define DWSQ_FLAG_URGE			0x10	// 紧急定位,要占用2倍频度
#define DWSQ_FLAG_COMM			0x00	// 普通定位

#define DWSQ_FLAG_HEIGHT_HAVE	0x00	// 有测高
#define DWSQ_FLAG_HEIGHT_NO		0x01	// 无测高
#define DWSQ_FLAG_HEIGHT_1		0x02	// 测高1
#define DWSQ_FLAG_HEIGHT_2		0x03	// 测高2

#define TX_FLAG_CHN	0
#define TX_FLAG_BCD	1
#define TX_FLAG_MIX	2	// 扩展通信协议支持

// 通信申请
typedef struct tagUSET_TXSQ
{
	tagUSET_TXSQ(){memset(this, 0, sizeof(tagUSET_TXSQ));};
	UINT8	Flag;				// 信息类别：0=汉字，1=代码，2=混合(扩展通信支持)3:zhitong 
	UINT8	Fast;				// ****是否特快0=特快，1=普通
	UINT32  DestID;				// 接收端ID,如果使用的是指挥卡,且该地址为通播地址,则为广播,下辖用户均可收到
	UINT16  Len;				// 电文长度(Byte)
	char	Msg[421];			// 电文内容0xF1;
}USET_TXSQ,USET_TXA;

// 查询申请
// 用户的定位结果,收发的报文,收到后的回执,均会在总站保存一段时间,用户可以通过本命令进行查询.
//
// 定位查询:查询自己或者下辖用户机的最近几次定位结果(可能前面申请了,但没有收到)
// 按"最新"方式,可以查询所有用户发给自己的未收到最近的5次报文，每查询一次，返回1条 
// 按"发送地址"方式,可以查询指定用户发给自己的未收到的报文                           
// 按"回执"方式,可以查询指定自己发给指定用户报文后,对方反馈给总站的"收到回执"
typedef struct tagUSET_CXSQ
{	
	tagUSET_CXSQ(){memset(this, 0, sizeof(tagUSET_CXSQ));};
	UINT32  UserID;				// 被查用户ID
	UINT8   Kind;				// 查询类别(0=定位,1=通信)	
	union 
	{
		UINT8   Count;			// 定位查询时为"定位次数":01=1次, 02=2次,       03=3次       
		UINT8   Style;			// 通信查询时为"查询方式":01=最新,02=按发方地址,03=回执    
	};
} USET_CXSQ,USET_CXA;

// 位置报告1(电文方式）
// 发送命令时,应用层填写该结构,主机用专门位置报告电文(而非通信扩展)发出
typedef struct tagUSET_WZBG1
{
	tagUSET_WZBG1(){memset(this, 0, sizeof(tagUSET_WZBG1));};
	UINT32  SendID;				// 发方ID
	UINT32  DestID;				// 收方ID
	UINT8   T[4];				// 报告时间
	UINT8   LSign;				// 经度符号（0=东经，1=西经）
	UINT8   L[4];				// 经度
	UINT8   BSign;				// 纬度符号（0=北纬，1=南纬）
	UINT8   B[4];				// 纬度
	float   Height;				// 高程（米）
} USET_WZBG1,USET_WAA;

// 位置报告2(定位方式--按“无高程/有天线高”定位）
// 主机发出时差和定位参数,总站解算,解算结果发送给收方
typedef struct tagUSET_WZBG2
{
	tagUSET_WZBG2(){memset(this, 0, sizeof(tagUSET_WZBG2));};
	UINT32  DestID;				// 收方ID
	UINT8   Freq;				// 报告频度
	float   AntHeight;			// 天线高(米）
} USET_WZBG2, USET_WBA;

// 口令识别(上级发送识别命令)
typedef struct tagUSET_KLML
{
	tagUSET_KLML(){memset(this, 0, sizeof(tagUSET_KLML));};
	UINT32	SubID;				// 下级地址
	char	Response;			// Y=应答，N=不应答
} USET_KLML, USET_KLS;
typedef USET_KLS  UGET_KLS;

// 口令识别(下级应答上级）
typedef struct tagUSET_KLYD
{   
	tagUSET_KLYD(){memset(this, 0, sizeof(tagUSET_KLYD));};
	UINT32	PeerID;				// 对方地址
	UINT8	Flag;				// 信息类别,0=汉字，1=代码，2=混合(扩展通信支持)
	UINT8	Len;				// 长度(字节)
	char    Msg[22];			// 内容
} USET_KLYD, USET_KLT;
// 口令识别应答(指挥机接收下级的应答)
typedef USET_KLT  UGET_KLT;

// 用户卡检测(获取用户信息)
typedef struct tagUSET_ICJC
{
	tagUSET_ICJC(){memset(this, 0, sizeof(tagUSET_ICJC));};
	UINT8    FrameNo;			// 帧号(0=本机,0~127=卡内下辖用户,128~255=外加非密用户)
	UINT8	 Num;				// 序号0-3
} USET_ICJC,USET_ICA;

// 设备管信设置，用于用户卡的初始化
typedef struct tagUSET_GLXX
{
	tagUSET_GLXX(){memset(this, 0, sizeof(tagUSET_GLXX));};
	UINT8   Mode;				// 类型：1=设置, 2=读取
	char	ManageInfo[64];		// 管理信息
} USET_GLXX,USET_GXM;

// 静默设置
typedef struct tagUSET_JMSZ
{
	tagUSET_JMSZ(){memset(this, 0, sizeof(tagUSET_JMSZ));};
	UINT8   Silience;			// 0=解除静默,1=静默1(禁止回执和入站),2=静默2(禁止回执，但可入站（通信/定位/定时）)
} USET_JMSZ,USET_JMS;

// 紧急自毁
// 只有在特殊场合下使用.一旦发送了该命令,主机便删除其中的所有软件和用户卡数据,以后不能再使用
typedef struct tagUSET_JJZH
{
	tagUSET_JJZH(){memset(this, 0, sizeof(tagUSET_JJZH));};
	UINT8   Reserved[4];		// 0xAA5555AA
} USET_JJZH,USET_ZHS;

// 保护门限
typedef struct tagUSET_APL
{   
	tagUSET_APL(){memset(this, 0, sizeof(tagUSET_APL));};
	STYPE	src;			
	UINT8	T[4];			// *UTC时间
	float	LevelH;			// 水平保护门限
	float	LevelV;			// 垂直保护门限
	float	Level3D;		// 空间保护门限
}USET_APL;
typedef USET_APL  UGET_APL ;

// 定位模式
// 在工作模式下，定位模式包括：1=BD-RNSS单频定位，2=双频定位，3=RDSS定位，4=GPS定位，5=兼容定位
typedef struct tagUSET_MSS
{
	tagUSET_MSS(){memset(this, 0, sizeof(tagUSET_MSS));};
	STYPE	src;			
	char	WorkMode;		// 工作模式：'C'=测试模式，'Z'=工作模式
	UINT8	PosMode;		// 定位模式: 0~9
	UINT8   Freq[3];		// 频点(0~5）{"","B1","B2","B3","L1","G1"}
	char	IQ[3];			// 支路（C=C码，P=P码，A=C/P两者)
} USET_MSS;
enum{MSS_B1_NONE = 0,MSS_B1 = 1,MSS_B2 = 2,MSS_B3 = 3,MSS_L1 = 4,MSS_G1 = 5};

// 设置波束，用于设置用户设备的响应波束和时差波束
typedef struct tagSET_BSS
{
	tagSET_BSS(){memset(this, 0, sizeof(tagSET_BSS));};
	UINT8  nResponseBeam;// 1.响应波束，用于产生发送信号的用户接受波束，取值范围0-10
	UINT8   nTimeDifBeam;// 2.时差波束，指用户设备双通道接受工作卫星进行时差测量的非响应波束，取值范围0-10
} USET_BSS;

// 系统自检
typedef struct tagSET_XTZJ
{
	tagSET_XTZJ(){memset(this, 0, sizeof(tagSET_XTZJ));};
	UINT32  srcID;
	UINT8   Freq;                // 频度
} USET_XTZJ;

#define MSS_FREQ_NONE	0
#define MSS_FREQ_B1		1
#define MSS_FREQ_B2		2
#define MSS_FREQ_B3		3
#define MSS_FREQ_L1		4	// GPS L1
#define MSS_FREQ_G1     5	// GLONASS L1

//----------------------------------------------------------------------------------------------
// 接收消息功能常数
//----------------------------------------------------------------------------------------------
enum BD2_COMM_UFUN_GET
{
	UFUN_GET_ZDA=BD2_UFUN_USER_START,	// 授时信息
	UFUN_GET_ZTI,						// 工作状态信息
	UFUN_GET_APL,						// 保护门限

	//RNSS
	UFUN_GET_GGA,					 	// 定位信息
	UFUN_GET_GSA,						// 当前卫星信息
	UFUN_GET_GSV,						// 可见卫星信息
	UFUN_GET_RMC,						// 推荐定位信息
	UFUN_GET_GLL,						// 定位地理信息
	UFUN_GET_VTG,						// 地面速度信息
	UFUN_GET_ICM,						// 解密模块有效期
	UFUN_GET_PMU,						// PRM模块有效期
    
	//RDSS
	UFUN_GET_DWR,						// 定位信息
	UFUN_GET_TXR,						// 通信信息
	UFUN_GET_HZR,						// 通信回执
	UFUN_GET_DWXJ,						// 兼收下属用户定位信息
	UFUN_GET_TXXJ,						// 兼收下属用户通信信息
	UFUN_GET_WAA,						// 位置报告1

	UFUN_GET_ICI,						// 本级用户卡信息
	UFUN_GET_ICZ,						// 下属用户卡信息

	UFUN_GET_GXM,						// 管理信息
	UFUN_GET_BSI,						// 功率状况
	UFUN_GET_FKI,						// 发射反馈
	
	UFUN_GET_KLT,						// 口令识别（下级应答）
	UFUN_GET_BID,						// 读取定位模式
	UFUN_GET_ZHR,						// 自毁确认信息
	UFUN_GET_DTY,                       // 设备信息
	UFUN_GET_RPQ,                       // 位置报告请求信息
	UFUN_GET_TSQ,                       // 态势发送
	UFUN_GET_FTX,                       // 自由通信
	UFUN_GET_WZB,                       // 位置报告
};

// 授时信息
typedef struct tagUGET_ZDA
{
	tagUGET_ZDA(){memset(this, 0, sizeof(tagUGET_ZDA));};
	STYPE	src;			
	UINT8   Mode;			// *授时模式，1=RDSS,2=RNSS
	UINT8   T[4];			// UTC时间
	UINT8	D[3];			// UTC日期
	INT8	LocalH;			// 本地时-HH(-13~+13,负=东经，正=西经）
	UINT8	LocalM;			// 本地时-MM(00~59分钟）
	UINT8	CorrectT[4];	// *时钟修正值时刻
	float	CorrectData;	// *修正值	
	UINT8	Precise;		// *精度指示（0=未知，1=0~10ns, 2=10~20ns,3〉20ns）
	char	Status;			// *卫星锁定状态（Y=锁定，N=失锁)
}UGET_ZDA;

// 状态信息
typedef struct tagUGET_ZTI
{
	tagUGET_ZTI(){memset(this, 0, sizeof(tagUGET_ZTI));};
	STYPE	src;			
	UINT8   PRM;			// PRM模块（0=正常，1=故障）
	UINT8   Encrypt;		// 加解密模块（0=正常，1=故障）
	UINT8	Annatena;		// 天线（0=正常，1=故障）
	UINT8	Channels;		// 通道（0=正常，1=故障）
	UINT8	OutPower;		// 内外电（0=电池，1=外电）
	UINT8	Recharged;		// 充电指示，0=充电，1=非充电
	float	RestPower;		// 剩余电量%
	UINT8	RestHM[2];		// 可用电量的可用时间（[0]=时，[1]=分）
}UGET_ZTI;

//----------------------------------------------------------------------------------------------
// RNSS-接收数据
//----------------------------------------------------------------------------------------------

// GGA,定位信息
// 状态指示：
//   GP定位时：0=不可用，1=SPS定位有效，2=差分定位有效，3=PPS定位有效，4=RTK定位有效，有整数解，5=RTK定位有效，有浮动解，6=估算结果，7=手动模式，8=模拟器模式
//   BD定位时：0=不可用，1=无差分定位有效，2=差分定位有效，3=双频定位有效
//   GN定位时：0=不可用，1=兼容定位有效
typedef struct tagUGET_GGA
{
	tagUGET_GGA(){memset(this, 0, sizeof(tagUGET_GGA));};
	STYPE	src;					// 来源
	UINT8   T[4];					// 时间
	UINT8   LSign;					// 经度符号（0=东经，1=西经）
	UINT8   L[5];					// 经度
	UINT8   BSign;					// 纬度符号（0=北纬，1=南纬）
	UINT8   B[5];					// 纬度
	float   Height;					// 高程（米）
	float   Segma;					// *高程异常(单位米,相对于CGC2000)
	UINT8   Flag;					// *状态指示(0~9）
	UINT8   SateCount;				// 使用的卫星数
	float   HDOP;					// 水平精度因子
	float   VDOP;					// *垂直精度因子
	UINT8   DiffTime;				// 差分数据龄期
	UINT16  DiffID;					// 差分ID号, 0000-1023
} UGET_GGA;

// GSA,当前卫星信息，参与解算的卫星
// 兼容定位时，每个星座单独用一条语句输出
typedef struct tagUGET_GSA
{
	tagUGET_GSA(){memset(this, 0, sizeof(tagUGET_GSA));};
	STYPE	src;					// 来源
	char    WorkMode;				// 工作模式, A=自动, M=人工
	UINT8   PosMode;				// 定位模式, 1=无法定位, 2=二维定位, 3=三维定位
	UINT8   Satellite[12];			// 卫星 #1---#12
	float   PDOP;					// 位置精度因子
	float   HDOP;					// 水平精度因子
	float   VDOP;					// 垂直精度因子
	float   TDOP;					// *时间精度因子
}UGET_GSA;

// GSV,可见卫星信息
typedef struct tagSAT_INFO
{
	tagSAT_INFO(){memset(this, 0, sizeof(tagSAT_INFO));};
	UINT8	SatNo;				// 卫星
	float	Elevation;			// 卫星的仰角0-90
	float	Azimuth;			// 卫星的方位角0-359
	float	fSNR;				// 信噪比
} SAT_INFO;

typedef struct tagUGET_GSV
{
	tagUGET_GSV(){memset(this, 0, sizeof(tagUGET_GSV));};
	STYPE	src;					// 来源
	UINT8   TotalInfo;				// 信息条数, 1-3
	UINT8   InfoID;					// 当前信息号 1-3
	UINT8   SatCount;				// 卫星总数(三条GSV报文中的卫星总数)
	SAT_INFO Satellite[4];
}UGET_GSV;

// RMC,推荐定位信息
typedef struct tagUGET_RMC
{
	tagUGET_RMC(){memset(this, 0, sizeof(tagUGET_RMC));};
	STYPE	 src;					// 来源
	UINT8	 D[3];                  // UTC日期
	UINT8	 T[4];                  // UTC时间
	UINT8	 LSign;				    // 经度符号（0=东经，1=西经）
	UINT8	 L[4];				    // 经度
	UINT8	 BSign;				    // 纬度符号（0=北纬，1=南纬）
	UINT8	 B[4];				    // 纬度
	float    Speed;                 // 地面速度(米/秒)
	float    FollowArc;             // 地面航向（相对于真北，顺时针为正）
	float    MagneticVariation;     // 磁偏角(负=西，正=东）
	char     Mode;                  // *模式指示(A=自主定位，D=差分定位，E=估算，N=无效)
}UGET_RMC;

// GLL，定位地理信息
typedef struct tagUGET_GLL
{
	tagUGET_GLL(){memset(this, 0, sizeof(tagUGET_GLL));};
	STYPE	src;					// 来源
	UINT8   L[4];					// 经度
	UINT8	LSign;				    // 经度符号（0=东经，1=西经）
	UINT8   B[4];                   // 纬度
	UINT8	BSign;				    // 纬度符号（0=北纬，1=南纬）
	UINT8   T[4];                   // UTC时间
	char    Mode;					// *模式指示（A=自动模式，D=差分，E=估算，M=手动输入，S=模拟器,N=无效）
}UGET_GLL;

// VDT,地面速度信息
typedef struct tagUGET_VTG
{
	tagUGET_VTG(){memset(this, 0, sizeof(tagUGET_VTG));};
	STYPE	  src;					// 来源
	float     RNC;					// 以真北为参考基准的地面航向（000~359度）
	float     MNC;					// 以磁北为参考基准的地面航向（000~359度）
	float     KTS;					// 地面速度（000.0~999.9节）
	float     KPH;					// 地面速度（0000.0~1851.8公里/小时)
	char      Mode;					// *模式指示（A=自主定位，D=差分定位，E=估算，M=手动，S=模拟器，N=无效）
}UGET_VTG;

// haige phb 2.1协议开始，无起始日期
// 解密模块的有效期
typedef struct tagUGET_ICM
{
	tagUGET_ICM(){memset(this, 0, sizeof(tagUGET_ICM));};
	STYPE	src;					// 来源
	UINT8   D1[3];					// 起始日期
	UINT8   D2[3];					// 结束日期
}UGET_ICM;

// PRM模块的有效期
typedef struct tagUGET_PMU
{
	tagUGET_PMU(){memset(this, 0, sizeof(tagUGET_PMU));};
	STYPE	src;					// 来源
	UINT8	Freq;					// B1/B2/B3
	UINT8   D1[3];					// 起始日期
	UINT8   D2[3];					// 截至日期
}UGET_PMU;

//----------------------------------------------------------------------------------------------
// RDSS-接收数据
//----------------------------------------------------------------------------------------------

// 定位信息(本机定位，定位查询，位置报告2，兼收下属定位）
// 本机定位时SendID=0
typedef struct tagUGET_DWXX
{
	tagUGET_DWXX(){memset(this, 0, sizeof(tagUGET_DWXX));};
	UINT32  SendID;			// 发方ID
	UINT8   Kind;			// 信息类别：1=定位结果，2=查询结果，3=位置报告(方式2)
	UINT8	Flag;			// 信息标志 定位精度指示 0一档，1二档
	UINT8   T[4];			// UTC时间
	UINT8   LSign;			// 经度符号（0=东经，1=西经）
	UINT8	L[4];			// 定位经度
	UINT8   BSign;			// 纬度符号（0=北纬，1=南纬）
	UINT8   B[4];			// 定位纬度
	float	Height;			// 高程(单位米)
	float   Segma;			// 高程异常(单位米,对高空用户,该字段为0)
} UGET_DWXX,UGET_DWR;

#define DWXX_FLAG_PRICISE2	0x08	// 为二档精度(100米),否则为一档精度(20米),
#define DWXX_FLAG_URGED		0x04	// 为紧急定位
#define DWXX_FLAG_MULTI_POS	0x02	// 为多值解
#define DWXX_FLAG_AIR		0x01	// 为高空定位

#define DWXX_KIND_POS	1			// 定位结果
#define DWXX_KIND_QRY	2			// 查询结果
#define DWXX_KIND_RPT	3			// 位置报告2

#define UGET_DWXJ UGET_DWR	// 监收下属定位

// 通信信息（本机接收，查询通信，监收下属通信）
typedef struct tagUGET_TXXX
{
	tagUGET_TXXX(){memset(this, 0, sizeof(tagUGET_TXXX));};
	UINT32	SendID;			// 发方ID
	UINT32	DestID;			// 收方ID
	UINT8	Kind;			// 信息类别:1=普通电文，2=特快，3=通播，4=按最新存入查询电文，5=按发方查询电文
	UINT8	Flag;			// 信息类别，0=汉字，1=BCD码，2=混合(扩展库支持)
	UINT8	H;				// 发送时间：小时,对通信查询的结果有效
	UINT8	M;				// 发送时间：分,对通信查询的结果有效
	UINT16	Len;			// 长度（字节)
	char	Msg[421];		// 内容(注意,没有在串后加'\0')
	UINT8	CRC;
}UGET_TXXX,UGET_TXR;

#define TXXX_KIND_GNL		1	// 普通
#define TXXX_KIND_FST		2	// 特快
#define TXXX_KIND_BRD		3	// 通播
#define TXXX_KIND_QRY_LST	4	// 按最新存入查询电文
#define TXXX_KIND_QRY_SND	5	// 按发方查询电文

typedef UGET_TXR  UGET_TXXJ ;	// 兼收下属通信

// 通信回执
// 回执查询申请的返回结果
typedef struct tagUGET_TXHZ
{
	tagUGET_TXHZ(){memset(this, 0, sizeof(tagUGET_TXHZ));};
	UINT32 DestID;			// 目标ID
	UINT8  Count;			// 回执个数,最多5个
	struct 
	{
		UINT8 SendHour;		// 发送时
		UINT8 SendMin;		// 发送分
		UINT8 AnswerHour;	// 应答时
		UINT8 AnswerMin;	// 应答分
	} Times[5];
}UGET_TXHZ,UGET_HZR;

// 位置报告1(电文方式）
typedef USET_WAA  UGET_WZBG1 ;
typedef USET_WAA  UGET_WAA   ;

// 本级用户卡信息
typedef struct tagUGET_ICBJ
{
	tagUGET_ICBJ(){memset(this, 0, sizeof(tagUGET_ICBJ));};
	UINT32	UserID;			  // 本机用户ID
	UINT32	SerialNo;		  // 序列号
	UINT32  BroadcastID;	  // 通播ID
	UINT8   Flag;			  // 用户特征,见下面的掩码
	UINT16  Freq;			  // 服务频度
	UINT8   Grade;			  // 通信等级(1~4)
	UINT8   Encrypt;		  // 加密标志(0=否,1=是)
	UINT16  UserTotal;		  // 下属用户总数(某些设备无效)
}UGET_ICBJ, UGET_ICI;

#define ICBJ_FLAG_USER  0x03		// 是普通用户卡,可以利用该掩码判断是否为指挥卡
#define ICBJ_FLAG_IDREG 0x04		// 需要身份认证

// 下级用户信息
// 每次最多返回40个
typedef struct tagUGET_ICXJ
{
	tagUGET_ICXJ(){memset(this, 0, sizeof(tagUGET_ICXJ));};
	UINT8     UserCount;    // 本帧用户数
	UINT32    UserIDs[40];	// 下辖用户
} UGET_ICXJ;

// 波束状况信息
typedef struct tagUGET_GLZK
{
	tagUGET_GLZK(){memset(this, 0, sizeof(tagUGET_GLZK));};
	UINT8	MainBeam;		// 主波束(0=没有锁定,0xff=未知)
	UINT8	SlaveBeam;		// 从波束(0=没有锁定,0xff=未知)
	UINT8	Beams[10];		// 每个通道的信号强度(0~4)
} UGET_GLZK,UGET_BSI;

// 管理信息
typedef struct tagUGET_GXM
{
	tagUGET_GXM(){memset(this, 0, sizeof(tagUGET_GXM));};
	UINT8	ManageInfo[64+1];	// 管理信息
}UGET_GLXX,UGET_GXM;

// 发射反馈信息
typedef struct tagUGET_FKXX
{
	tagUGET_FKXX(){memset(this, 0, sizeof(tagUGET_FKXX));};
	char	Cmd[4];			// 命令码(三个字符)
	UINT8	Result;			// 结果(0=成功，1=失败）
	UINT8	Freq;			// 频度：0=正确，1=超频
	UINT8	Launch;			// 发射指示:0=发射抑制解除，1=发射抑制，2=电力不足，3=无限静默
	UINT16	WaitTime;		// 等待时间(s)
} UGET_FKXX,UGET_FKI;

// 定位模式（GET_BID）
typedef struct tagGET_BID
{   
	tagGET_BID(){memset(this, 0, sizeof(tagGET_BID));};
	UINT8 nFrec; // 1=B1,2=B1B3,3=B3,4=B3L1G1,5=L1,6=G1,7=B1L1
	char  cCP;	 // 'C'=C码，'P'=P码，'A'=P码C码
} UGET_BID;

typedef struct tagGET_ZHR
{
	tagGET_ZHR(){memset(this, 0, sizeof(tagGET_ZHR));};
	char ZHInfo[16]; // "AA5555AA"表明自毁成功
} UGET_ZHR;

// 设备信息(RMO命令读取)
typedef struct tagSET_DTY
{
	tagSET_DTY(){memset(this, 0, sizeof(tagSET_DTY));};
	char	Vendor[16];	    // 厂家信息
	UINT8	Modal;          // 型号        0:简配型  1:标准型
} UGET_DTY;

//对应命令常量 UFUN_SET_SEC
typedef struct tagSET_SEC
{
	int nMode;
}USET_SEC,*LPUSET_SEC;

#pragma pack(pop)

#endif
