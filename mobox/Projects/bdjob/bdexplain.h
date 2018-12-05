//����������
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
	int nSerialNo;      //���к�
	int nBroadId;     //������ͨ�û���,Ϊ����ͨ��ID;�Ծ߱�ָ�ӹ��ܵ��û��豸,Ϊ����ͨ��ID��
	int nUsrAttribute; //�û�����ֵ:0:�ǻ����û���1:һ���û���2:�����û���3:�����û���4:ָ�����û���(���������֤)5:һ���û���(���������֤)6:�����û���(���������֤)7:�����û���(���������֤)
	int  nServiceFreq; //��λ:��
	BYTE byCommLevel;  //ͨ�ŵȼ�:1-4
	char chEncriptFlag;//���ܱ�ʶ��'E':����,'N':���ܡ�
	int  nSubUsersNum; //�����û���
}ICIInfo, *lpICIInfo;

typedef struct tagICAInfo
{
	BYTE byType;//0:��Ȿ���ӽ���ģ����Ϣ��1:���߱�ָ�ӹ����û��豸�������û���Ϣ��
	int  nLocalID;
	int  nSubUsrFrameNo;//��ָ�����͵���0ʱ�������û���Ϣ֡����0.
}ICAInfo, *pICAInfo;

typedef struct tagICZInfo
{
	int nTotalSubUserNums; //�������û���
	int nUsrCntInFrm;
	int nSubUserIds[100];
}ICZInfo, *lpICZInfo;

//ͨ����Ϣ�������� : CUNPACK_TXR 
typedef struct tagTXRInfo
{
	BYTE byInfoType;//��Ϣ���:1~5��1:��ͨͨ�ţ�2:�ؿ�ͨ�ţ�3:ͨ��ͨ��,4:�����´�����Ĳ�ѯ��õ�ͨ��,5:�����ŷ���ַ��ѯ��õ�ͨ�š�
	int  nLocalID;//���ŷ���ַ
	int  nDestionID; //���ŷ���ַ
	BYTE byTeleFormat;//������ʽ:0-���֣�1-���룬2-��ϴ���
	BYTE bySendHour;  //����ʱ��(ʱ)��byInfoType == 1��2��3ʱ������ʱ����Ч��byInfoType ==4��5ʱ,Ϊ����ѯ��ͨ�ŵ��������Ŀ���ϵͳ��¼�ķ���ʱ�䡣
	BYTE bySendMinute;//����ʱ��(��)��
	char chContent[1024];//ͨ�����ݡ�C������byTeleFormat == 2ʱ��������������ĸ�̶�ΪA4.
	int  nContentLen;
	BYTE byCRC; //CRCУ�飬0-��ȷ 1-����
}TXRInfo, *lpTXRInfo;


typedef struct tagFKIInfo
{
	char chCommandName[5];//ָ������:��"DWT"
	char chExeRslt;//ִ�����:'Y':ִ�гɹ�,'N':ִ��ʧ�ܡ�
	char chIntervalInd;//'Y':Ƶ����������,'N':Ƶ�����ô���.
	BYTE byTranInhabitInd;//��������ָʾ.0:�������ƽ����1:���յ�ϵͳ������ָ����䱻���ƣ�2:���ڵ������㣬���䱻����,3:�������ߵ羲Ĭ,���䱻���ơ�
	BYTE byMinuteWait;    //ָ����Ҫ�ȴ���ʱ��,�ֲ���
	BYTE bySecondWait;    //ָ����Ҫ�ȴ���ʱ��,�벿��
}FKIInfo, *lpFKIInfo;


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

//λ�ñ���1˫����� ��DEVICE_WAA
typedef struct tagWAAInfo
{
	int    nDestionID; //���շ�ID��������GJ 2016.3.29
	BYTE   byInfoType;//��Ϣ���͡�0-��ʾΪ�û��豸���յ�λ�ñ�����Ϣ����ʱnReportFreq��Ч��nUserIdΪ����λ�ñ�����û��ĵ�ַ��
	//1-��ʾΪ�û��豸���͵�λ�ñ�����Ϣ����ʱnUserIdΪ����λ�ñ�����Ϣ���û���ַ��
	int    nReportFreq; //����Ƶ�ȣ���λ:�롣
	int    nUserId;  //���ŷ�ID
	WORD   wReportHour;//λ�ñ���ʱ����Ϣ
	WORD   wReportMinute;
	float  fReportSecond;
	double dfLat;//γ�ȣ���λ:�ȡ�
	char   chLatDir;//'N'/'S';
	double dfLong;//���ȣ���λ:�ȡ�
	char   chLongDir;//'E'/'W';
	double dfHgt;//�߳�
	char   chHgtUnit;//�̵߳�λ:m
}WAAInfo, *lpWAAInfo;

//�����������ָ�� DEVICE_RMO
typedef struct tagRMOInfo
{
	char chName[4];//�Ϸ��ı�ʶ������"GGA"
	BYTE byMode;   //ģʽ��1:�ر�ָ����䣬2:��ָ����䣬3:�ر�ȫ����䣬4:��ȫ����䡣byMode == 3��byMode ==4ʱ:Ŀ��������ݱ�����Ϊ�ա�
	int  nOutFreq; //��λ:�롣byMode == 4ʱ,�˱�������Ϊ�ա�
}RMOInfo, *lpRMOInfo;

//�Լ���Ϣ
//CUNPACK_ZJXX
typedef struct tagSelfCheckInfo
{
	DWORD LocalID;
	BYTE ICCardState;		// IC��״̬
	BYTE HardState;		// Ӳ��״̬
	BYTE Batterystate;		// ��ص���
	BYTE InState;			// ��վ״̬
	BYTE PowerState[10];	// ����״��
}SelfCheckInfo, *lpSelfCheckInfo;

struct zti_st{
	BYTE   byStatusMask;//�ο�������������롣
	float  fPowerRemainPercent;//ʣ���ص����ٷֱ�
	BYTE   byPowerRemainMinute;//ʣ���������ʱ��,��λ:�֡�
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


//ͨ������ָ�� DEVICE_TXA     
typedef struct tagTXAInfo
{
	int nLocalId;   //�������������û���ID�ţ�������
	int  nUserId;   //�˴�ͨ�ŵ����ŷ���ַ��
	BYTE byCommCat; //ͨ�����0-�ؿ�ͨ�ţ�1-��ͨͨ�š�
	BYTE byCodeType;//���䷽ʽ��0-���֣�1-���룬2-��ϡ�
	bool ifcmd;   //����ʶ�� 0-ͨ�ţ�1-����ʶ��������
	bool ifAnswer; //�Ƿ�Ӧ�� ������ʶ��Ϊ0ʱ���Ƿ�Ӧ�������ȫ0  ��������
	char chContent[1024];//ͨ������.byCodeType ==0:ÿ������14bits,�Լ�������봫��;byCodeType==1,ÿ��bcd��ռ4bits���췢ʱ��ÿ���ֽ�ռ8bits
	int  nContentLen;    //ͨ�����ݵ��ֽ��������ĳ��ȣ�
	//         byCodeType ==1:ÿ��������һ��ASCII��ʾ��        
	//         byCodeType ==2:������������ĸ�̶�ΪA4��     
}TXAInfo, *lpTXAInfo;

//����״̬..
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
	int  GetInStationFreq();//�õ�����Ƶ��
	BOOL IsInit();
	BOOL IsNormal(); //�Ƿ���Է���
	BOOL IsOffline();//�豸��ʱ���ղ���BSI��Ϣ���Ѿ��£�
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
	DWORD   m_thid; //�߳�ID
	HANDLE m_thread;
	ICIInfo* m_icInfo;//����Ƶ��
	BOOL m_bNormal;//true�豸������û�й���
	time_t  m_activetime;
	
	CRITICAL_SECTION m_cs;
	
};

#endif