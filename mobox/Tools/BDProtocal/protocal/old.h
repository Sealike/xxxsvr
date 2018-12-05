#ifndef __STRUCT_H
#define __STRUCT_H
//////////////////////////////////////////////////////////////////////////
//���ļ�����������Ϣ���ͣ������Ķ�����ͬ������DllInterface.h
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

//����״̬..
typedef struct tagBeamInfo
{
	BYTE  byResponseNo;
	BYTE  byTimeDiffNo;
	BYTE  byPower[10];
}BeamInfo, *lpBeamInfo;

//��λ��Ϣ
//��λ��Ϣ, ��Ϣ���峣�� CUNPACK_DWR 
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

//IC��Ϣ CUNPACK_ICI
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


typedef struct tagKLSInfo
{   
	int  nSubUsrId;
	char chResponseFlag;

}KLSInfo, *lpKLSInfo;

//˫�����,ָ�����û��豸��������û����͵Ŀ���ʶ�����ݣ�������ͨ�豸��Ӧָ�����û��豸����ʶ��ָ�����Ϣ��
typedef struct tagKLTInfo
{   
	char chCharacter;//'P'/'Z'��'P':��ͨ���û��豸��Ӧָ�����û��豸����ʶ��ָ�����Ϣ��
	                 //         'Z':ָ�����û��豸��������û����͵Ŀ���ʶ�����ݡ�
	int  nUsrId;     //chCharacter==P:Ϊ���տ���ʶ����ϼ�ָ�����û�����'Z':�û���ַΪ�����û���ַ��
	BYTE byCommType; //0:���֣�1:����,2���췢��
	char chContent[512];//�������ݡ�

}KLTInfo, *LPKLTInfo,KLTStruct,*LPKLTStruct;

//��Ϣ����: CUNPACK_HZR;
typedef struct tagHZRInfo
{   
	int  nLocalId;       //�û���ַ��
	BYTE byResponseNum;  //��ִ��  :0 ~ 5;0:��ʾ�޻�ִ��
	WORD wSendHour[5];   //��Ӧ��ִ1~5�ķ���ʱ��
	WORD wSendMinute[5];
	WORD wAckHour[5];	 //��Ӧ��ִ1~5�Ļ�ִʱ��
	WORD wAckMinute[5];

}HZRInfo, *lpHZRInfo;


typedef struct tagPMUInfo
{   
	BYTE byFreq;  //Ƶ���:1,2,3
	WORD wBgnYear;//��ʼ����
	WORD wBgnMonth;
	WORD wBgnDay;
    
	WORD wEndYear;//��������
	WORD wEndMonth;
	WORD wEndDay;

}PMUInfo, *LPPMUInfo;

//CUNPACK_ICM
typedef struct tagICMInfo
{   
	WORD wEndYear;//��Կ��Ч��ʼ������ʱ����
	WORD wEndMonth;
	WORD wEndDay;

	WORD wBgnYear;//��Կ��Ч��������
	WORD wBgnMonth;
	WORD wBgnDay;
	
}ICMInfo, *LPICMInfo;

//
typedef struct tagPKYInfo
{   
	UINT uWeek;         //������0 :���ܡ�
	char chSecretKey[7];//��Կ
	char chKey[129];    //KEY
	char chB1Param[25]; //B1��������
	char chB2Param[25]; //B2��������
	char chB3Param[25]; //B3��������
    
}PKYInfo, *LPPKYInfo;

//CUNPACK_BSD;
typedef struct tagBSDInfo
{   
	char chName[21];    //��ʶ����;
	char chMapName[21]; //ͼ������;
	double dfLat;       //γ�ȡ���λ���ȣ�
	char   chLatNS;     //γ�ȷ���'N'/'S'
	double dfLong;      //���ȡ���λ:�ȡ�
	char   chLongEW;    //���ȷ���'E'/'W'
	double dfHgt;       //�̡߳�
	char   chHgtUnit;   //�̵߳�λ;
}BSDInfo, *LPBSDInfo;
//CUNPACK_BSQ;
typedef struct tagBSQInfo
{   
	char chName[21];    //��ʶ����;
	double dfCenterLat; //���ĵ�γ��,��λ:�ȡ�
	char   chLatNS;     //γ�ȷ���'N'/'S'
	double dfCenterLong;//���ĵ㾭��,��λ:��
	char   chLongEW;    //���ȷ���'E'/'W'
	double dfCenterHgt; //���ĵ�̡߳�
	char   chHgtUnit;   //�̵߳�λ;
	double dfRadius;    //����뾶
	char   chRadiusUnit;//�뾶��λ
}BSQInfo, *LPBSQInfo;


typedef struct tagPositionInfo
{   
	double dfLat;       //γ�ȡ���λ���ȣ�
	char   chLatNS;     //γ�ȷ���'N'/'S'
	double dfLong;      //���ȡ���λ:�ȡ�
	char   chLongEW;    //���ȷ���'E'/'W'
	double dfHgt;       //�̡߳�
	char   chHgtUnit;   //�̵߳�λ;

}PositionInfo, *LPPositionInfo;

//CUNPACK_BSX;DEVICE_BSX��˫����䡣
typedef struct tagBSXInfo
{   
	char chName[21];    //��ʶ����;
	WORD wTotalSentence;
	WORD wCurrSentenceNo;
	WORD wPointNum;
	tagPositionInfo posInfo[100];

}BSXInfo, *LPBSXInfo;

//�����䣬��������ͺ���; CUNPACK_ZTG

typedef struct tagVTGInfo
{   
	float fGrndDirNorth;//�Եط��򣬲ο��汱��
	char  chNorthRef;   //�汱�ο���
	float fGrndDirMagNorth;//�Եط��򣬲ο��ű���
	char  chMagNorthRef;//�ű��ο���
	float fGrndSpeedN;  //�Խ������ĵ���
	char  chSpeedUnitN; //�ٶȵ�λ����
	float fGrndSpeedKm; //��ǧ�������ĵ���
	char  chSpeedUnitK; //�ٶȵ�λ��km/h;
	char  chModeIndicate;//A:����ģʽ,B:���ģʽ,E:����ģʽ,M:�ֶ�����ģʽ,S:ģ����ģʽ,N:������Ч��

}VTGInfo, *LPVTGInfo;

//��Ӧ�ĳ��� CUNPACK_GBS
typedef struct tagGBSInfo
{   
	char  chSysType[3];//GN , GP, GA,BD,GL,.
	WORD  wPosHour  ;//��λʱ��(UTC)
	WORD  wPosMinute;
	float fPosSecond;
	
	float fLatPredicErr;//γ��Ԥ�����,��λ:�ס�
	float fLonPredicErr;//����Ԥ�����,��λ:�ס�
	float fHgtPredicErr;//�߳�Ԥ�����,��λ:�ס�
    
	BYTE  byFailedSat;//���Ƴ�������Ǻš���ʹ�ö������ϵͳʱ���������Ǻŷ�ΧΪ1~32,GPS���Ǻŷ�ΧΪ33~70; 
    float fProbability;//�ڹ���ʱ��©�����ǵĸ��ʡ�
    
	float fPredictSatCauseErr;//�������ǵĹ���ƫ�
	float fPredictStdErr;//ƫ�ù���ı�׼ƫ�
	
}GBSInfo, *LPGBSInfo;

//��Ӧ�ĳ��� CUNPACK_ECT
typedef struct tagECTInfo
{   
	char  chSysType[3];//GN , GP, GA,BD,GL,.
	BYTE  bySatNo;//���Ǻ�.
	char  chFreq[3];//Ƶ����Ϣ:"B1","B2","B3","S".
	BYTE  byChannelNo;//ͨ���š�
	char  chRoute;//֧·:'I','Q';
	char  chTeleInfo[256];//ԭʼ����������Ϣ��
	WORD  wTeleLen;//���ĳ���(�ַ���);
	
}ECTInfo, *LPECTInfo;

//��Ӧ�ĳ��� CUNPACK_GLL
typedef struct tagGLLInfo
{   
	char   chSysType[3];//GN , GP, GA,BD,GL,.
	double dfLat;      //γ��.��λ����
	char   chLatNS;    //γ�ȷ���'N'/'S'
	double dfLon;      //���ȡ���λ���ȡ�
	char   chLonEW;    //���ȷ���:'E'/'W'
	WORD   wPosHour;   //��λʱ��:UTC.
	WORD   wPosMinute;
	float  fPosSecond;
    char   chDataStatus;//����״̬:A-��Ч��V-��Ч��
	BYTE   byModeIndicate;//ģʽָʾ:0~5;0-�Զ�ģʽ��1-���ģʽ��2-���㣬3-�ֶ�����ģʽ��4-ģ����ģʽ��

}GLLInfo, *LPGLLInfo;
//DEVICE_MSS��������䡣�����û��豸��ǰ��λ��ʽ��
//������ģʽ(chModeCorZ)ȡ'C'ʱ��byTypeItem����Ĳ�����Ŀ��������:
//0:�����ʣ�1:��λ��2:������,3:������,4:������,5:��࣬6:��ʱ��7:�ز�,8:raim,9:λ�ñ���
//������ģʽ(chModeCorZ)ȡ'Z'ʱ��byTypeItem����Ĳ�����Ŀ��������:
//1:RNSS��Ƶ��λ,2:RNSS˫Ƶ��λ,3:RDSS��λ,4:GPS��λ��5:���ݶ�λ
//����BD-2ϵͳ��Ƶ��ȡֵΪ:"B1","B2","B3","S";����GPSϵͳ��Ƶ��ȡֵΪ"L1".
//֧·(chRoute_):"C":C��, "P":P��, "A":ȫ��֧·��
typedef struct tagMSSInfo
{   
	char chModeCorZ;    //'C':����ģʽ;'Z':����ģʽ
	BYTE byTypeItem;    // ��λģʽ/������Ŀ��
	char chFreq [3][3];    //B1��2��3��S,L1; *+'\0'�γ�c�Ĵ���
	char chRoute[3][3];    //C,P,A��* +'\0'�γ�c�Ĵ���
	char chSys[3][3];      //BD,GP,GL.
	
}MSSInfo, *LPMSSInfo;

//DEVICE_BDPMI��������䣬�����û��豸������ʡ��ģʽ��
typedef struct tagLPMInfo
{   
	BYTE byPowerSavingFlag;//0:ʡ��ģʽ,1:����ģʽ��

}LPMInfo, *LPLPMInfo;

//CUNPACK_ZDA;
//UTCʱ�䣬���ڣ��ͱ���ʱ������Ϣ ��Ϣ��������CUNPACK_ZDA 
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

//CUNPACK_GSA;
//��λ���Ǽ��������ӡ���
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

//CUNPACK_GXM;
//������Ϣ����˫�����
typedef struct tagGXMInfo
{
	int  nType; //1:�����û��豸������Ϣ 2:��ȡ�û��豸������Ϣ 3:�û��豸���صĹ�����Ϣ 
	int  nSerialNo;
	char chMagInfo[65];//16����ASCII�룬ָ������Ϊ2ʱ��������ϢΪ�ա�
	
}GXMInfo,*lpGXMInfo;

//CUNPACK_DHV;
//�ٶ���Ϣ����
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

typedef struct tagFKIInfo
{   
	char chCommandName[4];//ָ������:��"DWT"
	char chExeRslt       ;//ִ�����:'Y':ִ�гɹ�,'N':ִ��ʧ�ܡ�
	char chIntervalInd   ;//'Y':Ƶ����������,'N':Ƶ�����ô���.
	BYTE byTranInhabitInd;//��������ָʾ.0:�������ƽ����1:���յ�ϵͳ������ָ����䱻���ƣ�2:���ڵ������㣬���䱻����,3:�������ߵ羲Ĭ,���䱻���ơ�
	BYTE byMinuteWait;    //ָ����Ҫ�ȴ���ʱ��,�ֲ���
	BYTE bySecondWait;    //ָ����Ҫ�ȴ���ʱ��,�벿��
}FKIInfo, *lpFKIInfo;

typedef struct tagICZInfo
{   
	int nTotalSubUserNums;
	int nUsrCntInFrm;
	int nSubUserIds[40];
}ICZInfo, *lpICZInfo;

//ͨ����Ϣ�������� : CUNPACK_TXR 
typedef struct tagTXRInfo
{   
	BYTE byInfoType;//��Ϣ���:1~5��1:��ͨͨ�ţ�2:�ؿ�ͨ�ţ�3:ͨ��ͨ��,4:�����´�����Ĳ�ѯ��õ�ͨ��,5:�����ŷ���ַ��ѯ��õ�ͨ�š�
	int  nLocalID  ;//���ŷ���ַ
	BYTE byTeleFormat;//������ʽ:0-���֣�1-���룬2-��ϴ���
	BYTE bySendHour;  //����ʱ��(ʱ)��byInfoType == 1��2��3ʱ������ʱ����Ч��byInfoType ==4��5ʱ,Ϊ����ѯ��ͨ�ŵ��������Ŀ���ϵͳ��¼�ķ���ʱ�䡣
	BYTE bySendMinute;//����ʱ��(��)��
	char chContent[1024];//ͨ�����ݡ�C������byTeleFormat == 2ʱ��������������ĸ�̶�ΪA4.
	int  nContentLen;
}TXRInfo, *lpTXRInfo;

//˫����䣬�豸��ֵ��Ϣ����ȡ�豸��ֵָ������豸��ֵָ�
//��Ӧ������/��Ϣ������DEVICE_BDLZM/CUNPACK_LZM��
typedef struct tagLZMInfo
{   
	BYTE   byManageMode;//1-��ȡ�豸��ֵ��2-�����豸��ֵ��3-�����豸��ֵ��
	double dfZeroValue ;//byManageMode == 1ʱ���豸��ֵΪ�ա�

}LZMInfo, *LPLZMInfo;

//ͨ������ָ�� DEVICE_TXA     
typedef struct tagTXAInfo
{   
	int  nUserId;   //�˴�ͨ�ŵ����ŷ���ַ��
	BYTE byCommCat; //0-�ؿ�ͨ�ţ�1-��ͨͨ�š�
	BYTE byCodeType;//0-���֣�1-���룬2-��ϡ�
	char chContent[1024];//ͨ������.byCodeType ==0:ÿ������16bits,�Լ�������봫��;
	int  nContentLen;    //ͨ�����ݵ��ֽ�����
	//         byCodeType ==1:ÿ��������һ��ASCII��ʾ��        
	//         byCodeType ==2:������������ĸ�̶�ΪA4��     
}TXAInfo, *lpTXAInfo;

//λ�ñ���1˫����� ��DEVICE_WAA
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

//λ�ñ���2 ��վ��䡣��Ϣ�������� CUNPACK_WBA 
typedef struct tagWBAInfo
{   
	int    nUserId  ;  //����λ�ñ��淽���û���ַ��
	char   chHgtType;  //�߳�ָʾ:'H'-�߿�;'L'��ͨ��
	double dfAnteHgt;  //���߸ߡ���λ:�ס�
	float  fReportFreq;//����Ƶ��:�롣
}WBAInfo, *lpWBAInfo;

//�豸��ز���,��Ϣ�������� CUNPACK_SBX
typedef struct tagSBXInfo
{   
	char   chManufac[128];//�豸����������
	char   chDevType[128];//�豸����
	char   chSoftVer[128];//����汾��
	char   chAppProtocalVer[128];//����Э��汾��
	char   chICDVer[128];//ICDЭ��汾��
	char   chDevSerialNo[128];//�豸���к�
	int    nUserId;//ID��
}SBXInfo, *lpSBXInfo;

////CUNPACK_ZTI
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

typedef struct tagWZBGgx{
	UINT  uLocalId;
	BOOL  bOn;
	UINT  uRcvId;
	UINT  uFreq;//0��ʾ����λ�ñ���
	BYTE  byType;//0�� ͨ��	1����λ
	SYSTEMTIME T;
	double dfLat;
	double dfLon;
	double dfHgt;
	double dfAnteHgt;
}WZBGgx,*lpWZBGgx;

//������䣬�����û����ԭʼ������Ϣ����Ӧ������� :DEVICE_BDECS
typedef struct tagECSStruct
{
	char chFreq[3];  //Ƶ��š�"B1","B2","B3","S"��
	BYTE byChannelNo;//ͨ���š�"01"~"12"��
	char chRoute[3] ;//  ֧·��'I','Q','A'��
}ECSStruct ,*LPECSStruct ;

//DEVICE_BDGLS
typedef struct tagGLSStruct
{
	double dfLat;    //γ�� ��λ����
	char   chLatNS;  //γ�ȷ���:'N'/'S';
	double dfLon;    //���� ��λ���ȡ�
	char   chLonEW;  //γ�ȷ���:'E'/'W';
	float  fHgt;     //�߳�.�ο����ˮ׼�档
	char   chHgtUnit;//�̵߳�λ��
	char   chPrecisionIndicat;//����ָʾ.'A'/'P';'A'-����λ�ã��ռ�������1m~100km֮�䣻'P'-��ȷλ�ã��ռ����С��1�ס�
	int    nYear;    //��ǰ��UTC����ʱ�䡣
	WORD   wMonth;
	WORD   wDay;
	WORD   wHour;
	WORD   wMinute;
	WORD   wSecond;
	BYTE   byInitCat;//0-������Ч������Ϊ��/��������1-�������������Ϊ��������2-����洢��������Ϊ��������
}GLSStruct, *lpGLSStruct;

//�û�����λָ�
//�������:  ����峣��:DEVICE_BDRIS;
typedef struct tagRISStruct
{   
	DWORD dwReserved1;//not used!
	DWORD dwReserved2;//not used!
}RISStruct, *LPRISStruct;

//�û����Ի�ָ�
//�������:  ����峣��:DEVICE_BDZHS;
typedef struct tagZHSStruct
{   
	DWORD dwCmd;//�̶�����:0xAA5555AA,
}ZHSStruct, *LPZHSStruct;

/*
#define __BD   0 //����ϵͳ
#define __GPS  1 //GPSϵͳ
#define __GLO  2 //Glonassϵͳ
#define __GAL  3 //٤����ϵͳ

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
	int   nSatNo;//���Ǻ�
	int   nFreq; //Ƶ�� 0~3
	int   nSys;  //ϵͳ 0~3
	BYTE  byOper;//0:�ָ����� 1:�޳����ǣ�
}SatDelete,*LPSatDelete;

typedef struct tagOnePPSCorr
{   
	int    nSys;   //ϵͳ 0~3
	double dfValue;//��ֵ����λ���룻
}OnePPSCorr,*LPOnePPSCorr;

//���ʼ��
//#define DEVICE_GLJC  1
typedef struct tagPowerCheck
{
	DWORD LocalID;
	BYTE  Freq;			//��λ�����ӣ�0:�������
}PowerCheck, *lpPowerCheck;

//������λ����
//#define DEVICE_DWSQ  2
typedef struct tagPosReq
{   
	DWORD LocalID;
	BOOL  ifUrgent;		//0:��  1:������λ
	BOOL  ifTimeDiff;	//����ʱ��[�̶���0]
	BYTE  MeasureMode;	//00:�и߳�  01:�޲��  10:���1  11:���2
	BOOL  ifSecret;		//����[�̶�����]

	BOOL  ifHighAlt;	//[��׼Э��2.1���޴���] �߳�ָʾ[0:��ͨ  1:�߿�]
	int	  Altitude; 	//�߳�[��λ:��]
	DWORD Antena;		//���߸�
	DWORD Pressure;		//[��׼Э��2.1���޴���] ��ѹ����[�޷��ţ���λ:0.1Hp]
	short Temperature;	//[��׼Э��2.1���޴���] �¶�[��λ:0.1��]

	WORD PosFreq;		//��
}PosReq, *lpPosReq;

//ͨ������
//#define DEVICE_TXSQ  3
typedef struct tagCommReq
{	
	DWORD LocalID;
	DWORD DestAddress;		//���շ���ַ
	DWORD InfoLen;			//���ݳ���(��λ��Bit)  ���ܴ��ֽ�������ΪBCD���ܲ������ֽ�
	BYTE  InfoBuff[420+1];	//����:Max 120��  BCD:Max 420
	BYTE  GraphOrQuery;		//010:����ͨ��  011:��ѯͨ��
	//����ͨ��
	BOOL  ifSecret;			//�̶�����
	BOOL  ifQuick;			//00:��ͨ   01:�ؿ�[ע����Э��涨�෴]
	BOOL  ifBCD;			//0:����    1:����
	BOOL  ifCmd;			//[��׼Э��2.1���޴���] ����ʶ��[0:ͨ��  1:����ʶ��]
	BOOL  ifAnswer;			//[��׼Э��2.1���޴���] �Ƿ�Ӧ��
	//��ѯͨ��
	BOOL  QueryType;		//0:��λ��ѯ 1:ͨ�Ų�ѯ
	BYTE  QueryMode;		//��Ӧ��λ��ѯ:  00: 1��  01: 2��  10: 3��
	//��Ӧͨ�Ų�ѯ:  10:��ִ��ѯ  01:�����ŵ�ַ  00:�����´���ʱ��

	BYTE ifANS;				//װ������չЭ�飬�����ĵ��е��Ƿ�Ӧ���ֶ�
}CommReq, *lpCommReq;

//λ���ϱ�����
typedef struct tagPositionReportRecv
{
	DWORD LocalID ;   
	DWORD SrcAddress;

	//ʱ��
	BYTE Hour ;
	BYTE Minute ;
	BYTE Second ;
	BYTE Msel10 ;  //ʮ����Ϊ��λ
	//����
	BYTE LDegree ;
	BYTE LMinute ;
	BYTE LSecond ;
	BYTE LDecsec ;
	//γ��
	BYTE BDegree ;
	BYTE BMinute ;
	BYTE BSecond ;
	BYTE BDecsec ;

	//��λ��ʽ
	BYTE PosType ;
	int Alt ;
	int EF ;

}PositionReportRecv,*lpPositionReportRecv;

//��ʱ����
//#define DEVICE_DSSQ  4
typedef struct tagTimeReq
{
	DWORD LocalID;
	BOOL  ifHavePos;	//0:�޸���λ�� 1:�и���λ��
	BYTE  byType;
	//λ������ [2.1]
	WORD  Long;	//[��λ:0.1��]
	WORD  Lat;	//[��λ:0.1��]
	//λ������ [3.0]
	BYTE  Long_deg;
	BYTE  Long_min;
	BYTE  Long_sec;
	BYTE  Long_decsec;
	BYTE  Lat_deg;
	BYTE  Lat_min;
	BYTE  Lat_sec;
	BYTE  Lat_decsec;
	int   Altitude;
	int   UniZero;	//������ֵ[=������ֵ+�̶�ֵ193576ns]
	int   AppendZero;	//������ֵ
	BYTE  TimeSys;	//����趨[00: ����ʱ  01: UTC]

	WORD ServeFreq;//��վƵ��  (��λ:s)
}STimeReq, *lpSTimeReq;

//����ָ��
//#define  DEVICE_JSZL  5
typedef struct tagEndInstruct
{
	DWORD LocalID;
	DWORD dwVal;
}EndInstruct, *lpEndInstruct;

//�㲥��Ϣ
//#define  DEVICE_GBXX  6
typedef struct tagBroadInfo
{
	DWORD LocalID;
}BroadInfo, *lpBroadInfo;

//������Ϣ
//#define DEVICE_GYXX  7
typedef struct tagPublicInfo
{
	DWORD LocalID;
}PublicInfo, *lpPublicInfo;

//�������
//#define  DEVICE_WMCS  8
typedef struct tagErrCodeTest
{
	DWORD LocalID;
	BYTE  IQFrame;	//00:���I·����  01:���Q·����
	BYTE  BeamNo;	//001: 1����  010: 2����  011: 3���� 100: 4����  101: 5����  110:6����
}ErrCodeTest,*lpErrCodeTest;

//�����֤
//#define  DEVICE_ICJC  9
typedef struct tagReadCardReq
{
	DWORD LocalID ;
	BYTE  FrameNo ;
}ReadCard,*lpReadCardReq;

//�����Ի�
//#define  DEVICE_ZHSQ  10
typedef struct tagSelfDes
{
	DWORD LocalID;
}SelfDes, *lpSelfDes;

//��������
//#define  DEVICE_BSSZ  11
typedef struct tagBeamSet
{
	DWORD LocalID ;
	BYTE AnswerBeam ;		//��Ӧ����
	BYTE TimeDiffBeam;		// 0:�Զ�ѡ��1~6 Ϊ1~6������
} BeamSet,*lpBeamSet ;

//������ѯ
//#define DEVICE_BSCX   12
typedef struct tagBSCX_STRUCT
{
	int userAddr;
}BSCX_STRUCT,*pBSCX_STRUCT;

//��Ź�������
//#define DEVICE_GXZR   13
typedef struct tagCardSerialSet
{
	DWORD LocalID ;
	DWORD ICCardSerial ;		//���к�
	BYTE  ICManageInfo[65] ;	    // ������Ϣ
} CardSerialSet,*lpCardSerialSet;

//��Ŷ�ȡ
//#define  DEVICE_GXDQ  14 
//#define  DEVICE_XHDQ  19
typedef struct tagserielRead
{
	DWORD LocalID ; 
}SerielRead,*lpserielRead; 

//ϵͳ�Լ�
//#define  DEVICE_XTZJ  15
typedef struct tagSelfCheckReq
{	
	DWORD LocalID ;
	WORD  freq ;
}SelfCheckReq,*lpSelfCheckReq;

//��ֵ����
//#define  DEVICE_LZSZ  16
typedef struct tagZeroSet 
{
	DWORD LocalID ;
	DWORD ZeroValue ;	//��ֵ
} ZeroSet,*lpZeroSet;

//��ֵ��ȡ
//#define  DEVICE_LZDQ  17
typedef struct tagZeroRead
{
	DWORD LocalID ;
} ZeroRead,*lpZeroRead;

//ʱ�����
//#define  DEVICE_SJSC  18
typedef struct tagOutTime
{
	DWORD LocalID ;
	WORD freq ;
}OutTime, *lpOutTime ;

//�汾��ȡ 
//#define  DEVICE_BBDQ  20
typedef struct tagEditionRead
{   
	DWORD LocalID ;
}EditionRead,*lpEditionRead;

//��Ĭ����
//#define DEVICE_JMSZ   21
typedef struct tagSilent
{
	DWORD LocalID;
	BYTE  bSilent;
}Silent,*lpSilent;

//λ���ϱ�����
//#define  DEVICE_POSITIONREPORT  22
typedef struct tagPositionReport
{
	BOOL bReport2;//����ӣ��Ƿ���λ�ñ���2.
	DWORD LocalID ;
	DWORD DestAddress ;

	BYTE isHight ;
	int Alt ;
	int EF ;

	//ʱ��
	BYTE Hour ;
	BYTE Minute ;
	BYTE Second ;
	BYTE Msel10 ;  //ʮ����Ϊ��λ
	//����
	BYTE LDegree ;
	BYTE LMinute ;
	BYTE LSecond ;
	BYTE LDecsec ;
	//γ��
	BYTE BDegree ;
	BYTE BMinute ;
	BYTE BSecond ;
	BYTE BDecsec ;
}PositionReport,*lpPositionReport;
//ʱ�����
typedef struct tagTimeDiff
{
	DWORD LocalID;
	WORD  ServeFreq;//��λ: 31.25ms
}TimeDiff, *lpTimeDiff;

//ʱ�Ӷ�ȡ��Ϣ
typedef struct tagSZDQInfo
{
	DWORD LocalID ;
}SZDQInfo,*lpSZDQInfo;

//�������
typedef struct tagAsyComSet
{
	DWORD LocalID;
	BYTE  ComRate;	//��������(00: 19.2Kbps  01: 1.2  02: 2.4  03: 4.8  04: 9.6  05: 38.4[Default]  06:57.6  07:115.2) 
}AsyComSet, *lpAsyComSet;

//======����˫ϵͳ����Э��=======//

//GPS��λ����
typedef struct tagGpsReq
{
	DWORD LocalID;
	BYTE  Coordinate;	//[������˫ϵͳЭ��] 0:WGS84  1:BJ54
	BYTE  PosFreq;	//[������˫ϵͳЭ��] 0:���һ��  1��254:ÿ1��254���һ��  255:�����
}GpsReq, *lpGpsReq;

//======��չЭ��=======//

//���û��ϱ�����
typedef struct tagChildrenReq
{
	BYTE  ControlCode;	//1����ʾָ�����û�����PCƽ̨����һ�����û��ϱ�
}ChildrenReq, *lpChildrenReq;

//��·ά��
typedef struct tagLoopCheck
{
	BYTE  SequenceNum;//����˳���
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


//��������
typedef struct tagNetCardSet
{
	BYTE  MACAddr[6];	//�û������������ַ
	DWORD LocalIPAddr;	//PCƽ̨IP��ַ
	WORD  LocalPort;	//PCƽ̨�˿ں�
	DWORD PeerIPAddr;	//�û���IP��ַ
	WORD  PeerPort;		//�û����˿ں�
	DWORD SubNetMask;	//��������
}NetCardSet, *lpNetCardSet;



//======ͨ����չЭ��=======//

//�������з�
typedef struct tagGraphSplitReq
{
	DWORD LocalID;	//���ͷ���ַ
	DWORD DestAddress;	//���շ���ַ	
	BYTE  GraphID;	//���ı�ʶ��
	BYTE  TotalPackNum;	//�����ܰ���
	BYTE  CurPackNum;	//��ǰ�ְ�˳���
	DWORD InfoLen;	//���ĳ���(��λ���ֽ�)
	BYTE  InfoBuff[420+1];//����:Max 120��  BCD:Max 420
	BOOL  ifSecret;	//�̶�����
	BOOL  ifQuick;	//00:��ͨ  01:�ؿ�
	BOOL  ifBCD;	//0:����  1:����
}GraphSplitReq, *lpGraphSplitReq;

//=========================//
//======�û���-->����=======//
//=========================//


//����״��
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

//������λ��Ϣ
//CUNPACK_DWXX
typedef struct  tagPosInfo
{
	DWORD LocalID;	//����ID(���շ�ID�������û�)
	BOOL  InfoType;	//0:��λ  1:��ѯ
	BOOL  ifSecret;	//0:����Կ  1:����Կ
	BOOL  Precision;	//0:һ��(20m)  1:����(100m)
	BOOL  ifUrgent;		//0:��  1:����
	BOOL  ifMultiValue;	//0:��  1:��ֵ�� (��ʾ��֡����Ķ�λ��Ϣ�Ƿ�Ϊ��ȷ�Ķ�λ��Ϣ)
	BOOL  ifHighAlt;	//[��׼Э��2.1���޴���] �߳�ָʾ[0:��ͨ  1:�߿�]
	DWORD QueryAddress;	//ָ�����û�����ѯ�û����û���ַ
	SYSTEMTIME PosTime;	//��λʱ��
	BYTE  LongDeg;	//���ȣ���
	BYTE  LongMin;	//��
	BYTE  LongSec;	//��
	BYTE  LongDecsec;	//0.1��
	BYTE  LatDeg;	//γ�ȣ���
	BYTE  LatMin;	//��
	BYTE  LatSec;	//��
	BYTE  LatDecsec;	//0.1��
	short Altitude;	//�߳�
	short Ef;		//�߳��쳣
}PosInfo, *lpPosInfo;

//ͨ����Ϣ
//CUNPACK_TXXX
typedef struct tagCommInfo
{
	DWORD LocalID;	//����ID(���շ�ID�������û�)
	BOOL  ifBCD;	//0:����  1:����
	BOOL  InfoType;	//0:ͨ��  1:��ѯ
	BOOL  ifSecret;	//0:����Կ 1:����Կ
	DWORD SrcAddress;	//���ŷ���ַ
	BYTE  CommHour;	//ͨ����Ϣʱ����Ϊ0
	BYTE  CommMin;	//ͨ����Ϣʱ����Ϊ0
	DWORD CommLen;	//���ĳ���(��λ��bit)  ���ܴ��ֽ�������ΪBCD���ܲ������ֽ�
	BYTE  CommBuff[420+1];
	BOOL  CRCFlag;	//00:CRCУ����ȷ 01:CRCУ�����
}CommInfo, *lpCommInfo;

//ͨ�Ż�ִ
//CUNPACK_TXHZ
typedef struct tagCommAck
{
	DWORD LocalID;	//����ID
	DWORD DestAddress;	//���ŷ���ַ
	BYTE  AckCount;	//��ִ��
	BYTE  SendGraphHour[5];	//����ʱ��
	BYTE  SendGraphMin[5];
	BYTE  AckHour[5];	//����ʱ��
	BYTE  AckMin[5];

	BOOL bFailed;
	BYTE byFailedCode;
	BYTE byFailedTime[6];

}CommAck, *lpCommAck;

//����ʶ��
//CUNPACK_KLSB
typedef struct tagPswIdentify
{
	DWORD LocalID;	//����ID(���շ�ID�������û�)
	BOOL  ifBCD;	//0:����  1:����
	BOOL  ifSecret;	//0:����Կ 1:����Կ
	BYTE  PswAck;	//����Ӧ��
	DWORD CommLen;	//���ĳ���(��λ��bit)  ���ܴ��ֽ�������ΪBCD���ܲ������ֽ�
	BYTE  CommBuff[21+1];
}PswIdentify, *lpPswIdentify;

//��ʱ���[˫����ʱ]
//CUNPACK_DSJG
typedef struct tagTimeRes
{
	DWORD LocalID;
	SYSTEMTIME  ExactTime;
	BYTE  BeamNo;	//[����Э��2.1���޴���] ��ǰ������[001��110:1��6����]
	BYTE  BeamPower;	//[����Э��2.1���޴���] �źŹ���
	BOOL  LocalStatus;	//[����Э��2.1���޴���] ����״̬[00:����  01:�쳣]
	BOOL  ifBeamLock;	//[����Э��2.1���޴���] ��������[00:����  01:ʧ��]
	BOOL  SatelliteStatus;	//[����Э��2.1���޴���] ����״̬[00:����  01:�쳣]
	BOOL  CRCFlag;	//[����Э��2.1���޴���] CRCУ��[00:��ȷ  01:����]
	BOOL  TimeSys;	//[����Э��2.1���޴���] ����[00:����ʱ  01:UTCʱ]
	DWORD UniZero;	//[����Э��2.1���޴���] ������ֵ
	BYTE  LongDeg;		//���ȣ���
	BYTE  LongMin;		//��
	BYTE  LongSec;		//��
	BYTE  LongDecsec;	//0.1��
	BYTE  LatDeg;		//γ�ȣ���
	BYTE  LatMin;		//��
	BYTE  LatSec;		//��
	BYTE  LatDecsec;	//0.1��
	int	  Altitude;
	int   Revise;	//[����Э��2.1���޴���] ����ֵ[�з��ţ���λ:1����]
}TimeRes, *lpTimeRes;

//ʱ������
//CUNPACK_SCSJ
typedef struct tagTimeDiffData
{
	DWORD LocalID;
	int   TimeDiffValue;	//[�з��ţ���λ:1����]
}TimeDiffData, *lpTimeDiffData;

//��ǰ����
typedef struct tagCurStatus
{
	DWORD	LocalID;
	BYTE	CheckType;	//00:�Լ�  01:Ѳ��
	BOOL	Status;		//00:����  01:�쳣
}CurStatus, *lpCurStatus;

//�㲥���[����Э��3.0��]
//CUNPACK_GBSC
typedef struct tagBroadRes
{
	DWORD LocalID;
	DWORD Time;	    //����ϵͳʱ��[��λ��1����]
	BYTE  LeapSec;	//����[��λ��1��]
	int   TimeDiffer;	//ʱ��[�з��ţ���λ��10����]
	BYTE  AntennaNum;	//���ߺ�
	int   SatlX;	//����λ�ò���[�з��ţ���λ��1��]
	int   SatlY;
	int   SatlZ;
	short SpeedX;	//�����ٶȲ���[�з��ţ���λ��0.01��/��]
	short SpeedY;
	short SpeedZ;
	DWORD TimeDelay;	//ʱ��[��λ��1����]
	short a[17];	//�粨��������ģ�Ͳ���[�з��ţ���λ��1����]
	BYTE  YearCount;	//�����[2000���Ϊ0]
	BYTE  LeapSecStatus;	//����״̬[00:������  01:������  02:������]
	BYTE  BeamStatus;	//����״̬[1101:����  1110:����  1111:������  0000:����������]
	BYTE  SysStatus[5];	//ϵͳ״ָ̬ʾ(����)
}BroadRes, *lpBroadRes;

//�������
//CUNPACK_GYSC
typedef struct tagCommonRes
{
	DWORD LocalID;
	BYTE  Len;
	BYTE  CommonInfo[48];	//������Ϣ(384bit)
}CommonRes, *lpCommonRes;

//I·��Ϣ
//CUNPACK_ILXX
typedef struct  tagIFrameInfo
{
	BYTE BeamNo;	//������
	BYTE IDataSeg[30];	//I·���ݶ�(221bit) [����Э��2.1��]����֡�š����á��㲥�����ơ�ID1��ID2��I·��Ϣ���I·���ݶ���
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

//Q·��Ϣ
//CUNPACK_QLXX
typedef struct  tagQFrameInfo
{
	BYTE BeamNo;	//������
	BYTE QDataSeg[30];	//Q·���ݶ�(228bit) [����Э��2.1��]��Q·��Ϣ���Q·���ݶ���
}QFrameInfo, *lpQFrameInfo;

//������Ϣ
//CUNPACK_ICXX
typedef struct tagCardInfoRead
{
	DWORD LocalID ;
	BYTE FrameNum ;				//����
	//��0֡�����õ��ı���
	DWORD BroadcastID ;			//ͨ��ID
	BYTE MachineCharacter ;		//�û�������
	WORD ServeFreq ;			//����Ƶ��
	WORD CommLen;
	BYTE CommuLeave ;			//ͨ�ż���
	BYTE EncryptFlag ;			//���ܱ�־
	WORD SubordinateNum ;		//�¼��û�����
	//��N֡�����õ��ı���
	BYTE CurrFrameSubNum;		//��֡�����û���
	DWORD lpSubUserAddrs[100] ;	//ָ���¼��û���ַ��ָ��
} CardInfoRead,*lpCardInfoRead;

//�Ի�ȷ��
//CUNPACK_ZHQR
typedef struct  tagConfirmSelfDes
{
	DWORD LocalID ;
	DWORD DesFlag ;
}ConfirmSelfDes,*lpConfirmSelfDes;


//�Լ���Ϣ
//CUNPACK_ZJXX
typedef struct tagSelfCheckInfo
{
	DWORD LocalID ;
	BYTE ICCardState ;		// IC��״̬
	BYTE HardState ;		// Ӳ��״̬
	BYTE Batterystate ;		// ��ص���
	BYTE InState ;			// ��վ״̬
	BYTE PowerState[10] ;	// ����״��
}SelfCheckInfo,*lpSelfCheckInfo;

//��ֵ��Ϣ
//CUNPACK_LZXX
typedef struct tagZeroInfo
{
	DWORD LocalID ;
	DWORD ZeroValue ;
}ZeroInfo,*lpZeroInfo;

//ʱ����Ϣ
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

//�汾��Ϣ
//CUNPACK_BBXX
typedef struct tagEditionInfo
{
	DWORD LocalID ;
	int Num ;
	BYTE info[492] ;  //�汾��Ϣ���Զ��ŷָ�����Զ�������
}EditionInfo,*lpEditionInfo;


//�����Ϣ
//CUNPACK_XHXX
typedef struct tagSerialInfo
{
	DWORD LocalID ;
	DWORD SerialNum ;
	BYTE ManageInfo[65] ;
}SerialNum,*lpSerialNum;

//������Ϣ
//CUNPACK_FKXX
typedef struct tagFeedbackInfo
{
	DWORD LocalID ;
	BYTE FeedResult; //�������
	char Reserve[4] ;   // ����
}FeedbackInfo,*lpFeedbackInfo;


//GPS��λ��Ϣ
typedef struct tagGpsInfo
{
	DWORD LocalID;
	SYSTEMTIME PosTime;
	BYTE  LongDeg;	//���ȣ���
	BYTE  LongMin;	//��
	BYTE  LongSec;	//��
	BYTE  LongDecsec;	//0.1��
	BYTE  LatDeg;	//γ�ȣ���
	BYTE  LatMin;	//��
	BYTE  LatSec;	//��
	BYTE  LatDecsec;	//0.1��
	short Altitude;	//�߶�(��λ����)
	WORD  Speed;	//�ٶ�(��λ��0.01m/s)
	WORD  Direction;//����(��λ��0.01��)
	BYTE  Status;	//״̬
}GpsInfo, *lpGpsInfo;

//======��չЭ��=======//

//��·ά����Ӧ
typedef struct tagLoopAck
{
	BYTE  SequenceNum;//����˳���
}LoopAck, *lpLoopAck;

//�澯��Ϣ
typedef struct tagAlarmInfo
{
	BYTE  AlarmCode;	//�澯��: F0H:��Դ�澯, F1H��DSPͨ·����, F2H��DSP��, F3H���ź�ʧ��
}AlarmInfo, *lpAlarmInfo;

//ϵͳ����
typedef struct tagSysRestrain
{
	BYTE  RestrainFlag;//00:����ϵͳ���� 01:ϵͳ����
}SysRestrain, *lpSysRestrain;

//��Դ���ָʾ
typedef struct tagPowerInfo
{
	BYTE	ElecCapacity;	//����ָʾ0-5����ָʾ
}PowerInfo, *lpPowerInfo;

//����ָʾ
typedef struct tagEmitIndication
{
	BYTE  IndicateCode;		//����ָʾ��
}EmitIndication, *lpEmitIndication;

//Уʱ��Ϣ(������ʱ)
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
	int beamNo  ;//��Ӧ����
	int beamDiff;//ʱ���
}BSSZ_STRUCT,*pBSSZ_STRUCT;

//DEVICE_BDJMS
typedef struct tagBDJMS_STRUCT
{
	BOOL bInhabit;//�Ƿ�Ĭ
}BDJMS_STRUCT,*pBDJMS_STRUCT;

//DEVICE_BDICA...
typedef struct tagBDICA_STRUCT
{   
	BYTE byType;//0:��Ȿ���ӽ���ģ����Ϣ��1:���߱�ָ�ӹ����û��豸�������û���Ϣ��
	int  nSubUsrFrameNo;//��ָ�����͵���0ʱ�������û���Ϣ֡����0.
}BDICA_STRUCT,*pBDICA_STRUCT;
typedef BDICA_STRUCT ICAInfo;

//����ע��ָ�� �� DEVICE_BDGXM
typedef struct tagGXZR 
{
	BYTE byType;
	BYTE manageInfo[65];
	UINT uSerialNumber;
}GXZR_STRUCT,*pGXZR_STRUCT;

typedef struct tagDWSQ_STRUCT
{
	BOOL bifUrgency;
	BYTE bPosType;//0:�и߳�1���޸̣߳�2�����1��3���2��
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

}BDDWA_STRUCT,*pBDDWA_STRUCT;

typedef BDDWA_STRUCT   DWAInfo;
typedef pBDDWA_STRUCT  lpDWAInfo;

//��ʱ���� DEVICE_BDDSA
typedef struct tagDSSQ_STRUCT
{
	BOOL bifHavePos;//TRUE:�и���λ�ã�FALSE�޸���λ�ã���ʱ��γ��Ϊ�ա�
	BYTE bPosInfo;//no use...
	int  nType;//����1;˫��2.
	
	int userAddr;
	double fL;
	double fB;
	double fH;
	int iUniZero;//������ֵ����˫��ʱ����ʱ���ò�����0��
	int iAddZero;//ָ��ӵ��µȶ���ʱ��ɵĸ�����ֵ��
	BYTE bOutSet;//no use...
	DWORD dwFreq;
}DSSQ_STRUCT,*pDSSQ_STRUCT;

//ͨ�Ų�ѯ DEVICE_BDCXA
typedef struct tagBDCXA_STRUCT
{   
	BYTE byQueryCat;//0:��λ��ѯ;1ͨ�Ų�ѯ
	BYTE byQueryType;//��ѯ���Ϊ0ʱ��1��һ�ζ�λ��ѯ��2�����ζ�λ��ѯ��3�����ζ�λ��ѯ
					 //��ѯ���Ϊ1ʱ��1�����´�����Ĳ�ѯ���û���ַΪ�գ�2�������ŵ�ַ��ѯ���û���ַΪ���ŷ���ַ��3����ִ��ѯ���û���ַΪ���ŷ���ַ
	int  nUserId;
}BDCXA_STRUCT,*lpBDCXA_STRUCT;

//�����������ָ�� DEVICE_RMO
typedef struct tagRMOInfo
{   
	char chName[4];//�Ϸ��ı�ʶ������"GGA"
	BYTE byMode;   //ģʽ��1:�ر�ָ����䣬2:��ָ����䣬3:�ر�ȫ����䣬4:��ȫ����䡣byMode == 3��byMode ==4ʱ:Ŀ��������ݱ�����Ϊ�ա�
	int  nOutFreq; //��λ:�롣byMode == 4ʱ,�˱�������Ϊ�ա�
}RMOInfo,*lpRMOInfo;

typedef struct tagSubUserPos
{
	int  nUsrId;
	BYTE byPrecisionInd;//0:һ�� 1:����
	BOOL bUrgent;
	BOOL bMutiVal;
	BOOL bGaoKong;//0:��ͨ�û���1:�߿��û���

	BYTE  byPosHour;//��λʱ��
	BYTE  byPosMin;
	float fPosSec;
	
	double dfLat;//γ�ȣ���λ��
	double dfLong;//���ȣ���λ��;
	float  fHgt;//�̣߳���λ����
	float  fHgtXcept;//�߳��쳣����λ����
}SubUserPos ,*LPSubUserPos;

typedef struct tagSubUserMsg
{   
	int  nRcvrId;
	int  nSendId;
	BYTE byCodeType;//0 : ���֣�1:���룬2:�췢
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
	//utcʱ��
	int     nHour;
	int     nMinute;
	float   fSecond;	
	float	LevelH;			// ˮƽ��������
	float	LevelV;			// ��ֱ��������
	float	Level3D;		// �ռ䱣������	
}APLCmd,*LPAPLCmd;

typedef tagAPLCmd  tagAPLInfo;
typedef tagAPLCmd  APLInfo;
typedef tagAPLCmd *LPAPLInfo;


//nMode == 0 : 1pps
//nMode == 1 : B��
typedef struct SecondPulseMode
{
	int nMode;
}PulseMode,*LPPulseMode;

// ��λģʽ��GET_BID��
typedef struct tagBIDInfo
{   	
	UINT8 nFrec; // 1=B1,2=B1B3,3=B3,4=B3L1G1,5=L1,6=G1,7=B1L1
	char  cCP;	 // 'C'=C�룬'P'=P�룬'A'=P��C��
} BIDInfo,*LPBIDInfo;


typedef struct tagZHRInfo
{	
	char ZHInfo[16]; // "AA5555AA"�����Իٳɹ�
} ZHRInfo,*LPZHRInfo;

//һ����չЭ��ṹ�壬̬�Ʊ���
typedef struct UserPosition
{
	DWORD UserID;		//�û���ַ
	BYTE Day;			//��λʱ�̵�������
	BYTE Hour;			//��λʱ�̵�Сʱ����
	BYTE Min;			//��λʱ�̵ķ�����
	BYTE Second;		//��λʱ�̵�������
	BYTE DecSec;		//��λʱ�̵�.01������
	BYTE LongDeg;		//����
	BYTE LongMin;		//����
	BYTE LongSec;		//����
	BYTE LongDecsec;	//0.1��Ϊ��λ
	BYTE LatDeg;		//γ��
	BYTE LatMin;		//γ��
	BYTE LatSec;		//γ��
	BYTE LatDecsec;		//0.1��Ϊ��λ
}UserPosition,*lpUserPosition;

struct PosData
{
	UINT32 UserID;			// �û�ID
	UINT32 T;				// ʱ�䣬��(��+ʱ+��+��)
	float L;				// ���ȣ���
	float B;				// γ�ȣ���
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

//̬�����ݽ���
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
