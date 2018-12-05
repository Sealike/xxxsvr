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
		CUNPACK_BSI = 1,//������Ϣ1
		CUNPACK_DWR = 2,//��λ��Ϣ2
		CUNPACK_FKI = 3,//������Ϣ3
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
		//------------ͨ��Э����չ
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

	//����״̬..CUNPACK_BSI = 1
	typedef struct tagBeamInfo
	{
		BYTE  byResponseNo;
		BYTE  byTimeDiffNo;
		BYTE  byPower[10];
	}BeamInfo, *lpBeamInfo;

	//��λ��Ϣ, ��Ϣ���峣�� CUNPACK_DWR = 2
	typedef struct tagDWRInfo
	{   
		int    nLocalId;//nPosType==1:���ն˵�ID,==2:����ѯ�û����û���ַ,==3:����λ�ñ��淽���û���ַ��
		int    nPosType;//��λ��Ϣ���͡�1:���ն˶�λ���뷵�صĶ�λ��Ϣ,2:��λ��ѯ���ص��������û�λ����Ϣ��3:���յ���λ�ñ�����Ϣ��
		int    nPosHour;
		int    nPosMinute;
		float  fPosSecond;
		double dfLat;
		char   chLatDirection;//γ�ȷ���:'N'/'S';
		double dfLong;
		char   chLongDirection;//���ȷ���:'E'/'W';
		float  fEarthHgt;      //��ظ�.
		char   chHgtUnit;      //��ظߵ�λ��
		double dfHgtOut;       //�߳��쳣
		char   chHgtOutUnit;   //�߳��쳣��λ
		BYTE   byPrecisionInd; //��λ����ָʾ.0:һ��20�׾���;1:����100�׾���
		BOOL   bUrgent;        //�Ƿ����
		BOOL   bMultiValue;    //�Ƿ��ֵ��
		BOOL   bGaoKong;       //�߿�ָʾ

	}DWRInfo, *lpDWRInfo;

	//�ٶ���Ϣ��CUNPACK_FKI = 3
	typedef struct tagFKIInfo
	{   
		char chCommandName[5];//ָ������:��"DWT"
		char chExeRslt       ;//ִ�����:'Y':ִ�гɹ�,'N':ִ��ʧ�ܡ�
		char chIntervalInd   ;//'Y':Ƶ����������,'N':Ƶ�����ô���.
		BYTE byTranInhabitInd;//��������ָʾ.0:�������ƽ����1:���յ�ϵͳ������ָ����䱻���ƣ�2:���ڵ������㣬���䱻����,3:�������ߵ羲Ĭ,���䱻���ơ�
		BYTE byMinuteWait;    //ָ����Ҫ�ȴ���ʱ��,�ֲ���
		BYTE bySecondWait;    //ָ����Ҫ�ȴ���ʱ��,�벿��
	}FKIInfo, *lpFKIInfo;

	//IC��Ϣ CUNPACK_ICI = 4
	typedef struct tagICIInfo
	{   
		int nLocalId ;
		int nSerialNo;
		int nBroadId ;     //������ͨ�û���,Ϊ����ͨ��ID;�Ծ߱�ָ�ӹ��ܵ��û��豸,Ϊ����ͨ��ID��
		int nUsrAttribute; //�û�����ֵ:0:�ǻ����û���1:һ���û���2:�����û���3:�����û���4:ָ�����û���(���������֤)5:һ���û���(���������֤)6:�����û���(���������֤)7:�����û���(���������֤)

		int  nServiceFreq; //��λ:��
		BYTE byCommLevel;  //ͨ�ŵȼ�:1-4
		char chEncriptFlag;//���ܱ�ʶ��'E':����,'N':���ܡ�
		int  nSubUsersNum; //�����û���

	}ICIInfo, *lpICIInfo;
	 
	//ָ�ӻ��������û���Ϣ CUNPACK_ICZ = 5
	typedef struct tagICZInfo
	{   
		int nTotalSubUserNums;
		int nUsrCntInFrm;
		int nSubUserIds[40];
	}ICZInfo, *lpICZInfo;

	//UTCʱ�䣬���ڣ��ͱ���ʱ������Ϣ ��Ϣ��������CUNPACK_ZDA  = 11
	typedef struct tagZDAInfo
	{
		int    nMode;//ģʽָʾ:1~9��1:RDSS��ʱ���,2:RNSS��ʱ�����
		WORD   wYear;//UTC datetime
		WORD   wMonth;
		WORD   wDay;
		WORD   wHour;
		WORD   wMinute;
		float  fSeconds;
		int    nTimeArea;    //0 ~ [13,-13];����ʱ��(Сʱ�ӷ���(nLocalMinDiff),�Լ���ʾ�������ķ���)���ϱ���ʱ�伴��UTCʱ�䡣
		int    nLocalMinDiff;//0~59,��λ���ӡ�
		WORD   wModifyHour;  //��ʱ����ʱ��HHMMSS.SS
		WORD   wModifyMinute;
		float  fModifySeconds;
		double dfValueModify ;//����ֵ����λ:���롣
		int    nPrecisionFlag;//����ָʾ:0~3;0:δ��⣬1:0��10���룬2:10��20���룬3:����20���롣
		char   chSatStatus;   //�����ź�����״̬:'Y'/'N'��

	}ZDAInfo,*lpZDAInfo;

	//��λ���Ǽ��������ӡ���CUNPACK_GSA = 31
	typedef struct tagGSAInfo
	{
		char   chMode;//ģʽָʾ:M�ֶ���A�Զ���
		int    nModeOption;//1~3,1:��λ������,2:2D��3:3D
		int    nSatNo[12];//����PRN��:1~32,GPS:33~70 GLONASS : 71~102
		float  fPdop;
		float  fHdop;
		float  fVdop;
		float  fTdop;

	}GSAInfo,*lpGSAInfo;

	//�ٶ���Ϣ����CUNPACK_DHV = 65
	typedef struct tagDHVInfo
	{
		BYTE  byHour;
		BYTE  byMinute;
		float fSeconds;

		float fVel;//�ٶ�
		float velX;//X���ٶ�
		float velY;
		float velZ;
		float velGrnd;//�����ٶ�
		float velMax;//����ٶ�
		float velAve;//ƽ���ٶ�
		float velCourseAve;//ȫ��ƽ���ٶ�
		float velValid;
		char  chUnit;//��λ���Ƽ�Ϊ��	
	}DHVInfo,*lpDHVInfo;	

	//ͨ����Ϣ�������� : CUNPACK_TXR = 8
	typedef struct tagTXRInfo
	{   
		BYTE byInfoType;//��Ϣ���:1~5��1:��ͨͨ�ţ�2:�ؿ�ͨ�ţ�3:ͨ��ͨ��,4:�����´�����Ĳ�ѯ��õ�ͨ��,5:�����ŷ���ַ��ѯ��õ�ͨ�š�
		int  nLocalID  ;//���ŷ���ַ
		int  nDestionID;
		BYTE byTeleFormat;//������ʽ:0-���֣�1-���룬2-��ϴ���
		BYTE bySendHour;  //����ʱ��(ʱ)��byInfoType == 1��2��3ʱ������ʱ����Ч��byInfoType ==4��5ʱ,Ϊ����ѯ��ͨ�ŵ��������Ŀ���ϵͳ��¼�ķ���ʱ�䡣
		BYTE bySendMinute;//����ʱ��(��)��
		char chContent[1024];//ͨ�����ݡ�C������byTeleFormat == 2ʱ��������������ĸ�̶�ΪA4.
		int  nContentLen;
		BYTE byCRC;
	}TXRInfo, *lpTXRInfo;
	
	//ͨ������ָ�� DEVICE_TXA     
	typedef struct tagTXAInfo
	{   
		int nLocalId;   //�������������û���ID�ţ�������
		int  nUserId;   //�˴�ͨ�ŵ����ŷ���ַ��
		BYTE byCommCat; //ͨ�����0-�ؿ�ͨ�ţ�1-��ͨͨ�š�
		BYTE byCodeType;//���䷽ʽ��0-���֣�1-���룬2-��ϡ�
		bool ifcmd;   //����ʶ�� 0-ͨ�ţ�1-����ʶ��������
		bool ifAnswer; //�Ƿ�Ӧ�� ������ʶ��Ϊ0ʱ���Ƿ�Ӧ�������ȫ0  ��������
		char chContent[1024];//ͨ������.byCodeType ==0:ÿ������16bits,�Լ�������봫��;
		int  nContentLen;    //ͨ�����ݵ��ֽ��������ĳ��ȣ�
		//         byCodeType ==1:ÿ��������һ��ASCII��ʾ��        
		//         byCodeType ==2:������������ĸ�̶�ΪA4��     
	}TXAInfo, *lpTXAInfo;

	//CUNPACK_BDDWA ��λ����
	typedef struct tagBDDWA_STRUCT
	{
		int    userAddr;  //�û���ַ

		BOOL   bifUrgency;//����ָʾ
		BYTE   bPosType  ;//0:�и߳�1���޸̣߳�2�����1��3���2��
		BOOL   bifHigh   ;//�Ƿ�߿��û�

		double dfHgt     ;  //�߳�����  ��λ:��
		float  fAntennaHgt; //���߸ߡ�  ��λ:��
		double fAirPressure;//��ѹ���ݡ���λ:��
		double fTemperature;//�¶����ݡ���λ:��
		WORD   wFreq;       //��λƵ�ȡ���λ:��,0:���ζ�λ��
	}BDDWA_STRUCT,*pBDDWA_STRUCT,DWAInfo,*lpDWAInfo;

	//λ�ñ���1˫����� ��DEVICE_BDWAA = 29
	typedef struct tagWAAInfo
	{   
		BYTE   byInfoType;//��Ϣ���͡�0-��ʾΪ�û��豸���յ�λ�ñ�����Ϣ����ʱnReportFreq��Ч��nUserIdΪ����λ�ñ�����û��ĵ�ַ��
		//          1-��ʾΪ�û��豸���͵�λ�ñ�����Ϣ����ʱnUserIdΪ����λ�ñ�����Ϣ���û���ַ��
		int    nReportFreq; //����Ƶ�ȣ���λ:�롣
		int    nUserId;
		WORD   wReportHour;//λ�ñ���ʱ����Ϣ
		WORD   wReportMinute;
		float  fReportSecond;
		double dfLat;//γ�ȣ���λ:�ȡ�
		char   chLatDir;//'N'/'S';
		double dfLong;//���ȣ���λ:�ȡ�
		char   chLongDir;//'E'/'W';
		double dfHgt;//�߳�
		char   chHgtUnit;//�̵߳�λ��
	}WAAInfo, *lpWAAInfo;

	////CUNPACK_ZTI = 10
	typedef struct tagZTIInfo
	{   
		//#define MASK_CHARGE       0x01   0:���״̬ ��1:�ǳ��״̬
		//#define MASK_POWER_INOUT  0x02   0:���õ�� ��1:���õ�Դ
		//#define MASK_CHANNEL      0x04   0:������1���쳣
		//#define MASK_ANTENNA      0x08   0:������1���쳣
		//#define MASK_ENCRYPMODULE 0x10   0:������1���쳣
		//#define MASK_PRMMODULE    0x20   0:������1���쳣

		BYTE   byStatusMask;//�ο�������������롣
		float  fPowerRemainPercent;//ʣ���ص����ٷֱ�
		BYTE   byPowerRemainMinute;//ʣ���������ʱ��,��λ:�֡�
	}ZTIInfo, *lpZTIInfo;

	typedef struct  tagSatInfo
	{   
		int   nSatNo;      
		float fElevation;  //��������   ��λ:��
		float fOrientAngle;//���Ƿ�λ�� ��λ:��
		float fSnr;        //��������� ��λ:dB-Hz

	}SatInfo, *LPSatInfo;

	//�ɼ�������ϢCUNPACK_GSV = 18
	typedef struct  tagGSVInfo
	{   
		char    chSys[3];//BD+'\0', GP+'\0', GL+'\0', GA+'\0'.
		int     nSatVisible;
		SatInfo satInfo[4];
		int     nTotalStence;
		int     nCurrStence;
		int     nCurrSatNum;
	}GSVInfo, *LPGSVInfo;

	//RNSS ��λ������Ϣ CUNPACK_RMC = 17
	typedef struct  tagRMCInfo
	{
		char    chSys[3];//BD+'\0', GP+'\0', GL+'\0', GA+'\0',GN+'\0'.
		int     nYear ;
		int     nMonth;
		int     nDay  ;
		int		nPosHour;
		int		nPosMinute;
		float	fPosSecond;
		BYTE    byPosStatus;//״ָ̬ʾA/V;
		double  dfLong   ;//�ȡ�
		double  dfLat    ;//�ȡ�
		char    chLatNS  ;//γ�ȷ��� 'N'/ 'S'��
		char    chLongEW ;//���ȷ��� 'E'/ 'W'��
		float   fGrndVel ;//�����ٶȣ���λ���ڡ�
		float   fGrndDir ;//���溽�򣬵�λ���ȣ����汱Ϊ�ο���׼����˳ʱ�뷽��������ĽǶȡ�
		float   fMagAngle;//��ƫ��
		char    chMagDir ;//��ƫ�Ƿ���
		char    chModeFlag;//A:������λ��D:��֣�E:���㣬N:������Ч��
	}RMCInfo, *LPRMCInfo;

	//GNSS��λ��� CUNPACK_GGA = 16
	typedef struct  tagGGAInfo
	{ 
		char    chSys[3];//BD+'\0', GP+'\0', GL+'\0', GA+'\0',GN+'\0'.
		int		nPosHour;
		int		nPosMinute;
		float	fPosSecond;
		double  dfLong ;  //��
		double  dfLat  ;  //��
		char    chLatNS;  //γ�ȷ��� N S
		char    chLongEW; //���ȷ��� E W
		BYTE    byStatus; //״ָ̬ʾ0-8 a
		BYTE    bySatNum; //������ ��λ
		double  dfGndHigh;//��ظ�    
		char    chGndHighUint;//��ظ�  ��λ��
		double  dfHgtExcept;//�߳��쳣          
		char    chHgtExceptUint;   //�߳��쳣��λM*/
		unsigned short   DiffDate; //����������� xxxx
		char    chDiffStation[128];//���վ̨ID    
		float   fHdop;   
		double  dfVdop;
	}GGAInfo, *LPGGAInfo;

	//�����������ָ�� DEVICE_BDRMO = 25
	typedef struct tagRMOInfo
	{   
		char chName[4];//�Ϸ��ı�ʶ������"GGA"
		BYTE byMode;   //ģʽ��1:�ر�ָ����䣬2:��ָ����䣬3:�ر�ȫ����䣬4:��ȫ����䡣byMode == 3��byMode ==4ʱ:Ŀ��������ݱ�����Ϊ�ա�
		int  nOutFreq; //��λ:�롣byMode == 4ʱ,�˱�������Ϊ�ա�
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