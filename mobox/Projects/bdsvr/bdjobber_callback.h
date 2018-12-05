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



////����״̬..
//typedef struct tagBeamInfo
//{
//	BYTE  byResponseNo;
//	BYTE  byTimeDiffNo;
//	BYTE  byPower[10];		// �����źŵ�ǿ��
//}BeamInfo, *lpBeamInfo;

//����״̬..
typedef struct tagBeamInfo
{
	char  byPower[30];		// �����źŵ�ǿ��
	unsigned char  byResponseNo;
	unsigned char  byTimeDiffNo;
	//unsigned char  byPower[10];		// �����źŵ�ǿ��
	
}BeamInfo, *lpBeamInfo;

////λ�ñ���1˫����� ��DEVICE_WAA
//typedef struct tagWAAInfo
//{
//	int    nDestionID; //���շ�ID��������GJ 2016.3.29
//	BYTE   byInfoType;//��Ϣ���͡�0-��ʾΪ�û��豸���յ�λ�ñ�����Ϣ����ʱnReportFreq��Ч��nUserIdΪ����λ�ñ�����û��ĵ�ַ��
//	//1-��ʾΪ�û��豸���͵�λ�ñ�����Ϣ����ʱnUserIdΪ����λ�ñ�����Ϣ���û���ַ��
//	int    nReportFreq; //����Ƶ�ȣ���λ:�롣
//	int    nUserId;  //���ŷ�ID
//	WORD   wReportHour;//λ�ñ���ʱ����Ϣ
//	WORD   wReportMinute;
//	float  fReportSecond;
//	double dfLat;//γ�ȣ���λ:�ȡ�
//	char   chLatDir;//'N'/'S';
//	double dfLong;//���ȣ���λ:�ȡ�
//	char   chLongDir;//'E'/'W';
//	double dfHgt;//�߳�
//	char   chHgtUnit;//�̵߳�λ:m
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


//λ�ñ���1˫����� ��DEVICE_WAA
typedef struct tagWAAInfo
{
	int    nDestionID; //���շ�ID��������GJ 2016.3.29
	unsigned char   byInfoType;//��Ϣ���͡�0-��ʾΪ�û��豸���յ�λ�ñ�����Ϣ����ʱnReportFreq��Ч��nUserIdΪ����λ�ñ�����û��ĵ�ַ��
	//1-��ʾΪ�û��豸���͵�λ�ñ�����Ϣ����ʱnUserIdΪ����λ�ñ�����Ϣ���û���ַ��
	int    nReportFreq; //����Ƶ�ȣ���λ:�롣
	int    nUserId;  //���ŷ�ID
	unsigned char   wReportHour;//λ�ñ���ʱ����Ϣ
	unsigned char   wReportMinute;
	float  fReportSecond;
	double dfLat;//γ�ȣ���λ:�ȡ�
	char   chLatDir;//'N'/'S';
	double dfLong;//���ȣ���λ:�ȡ�
	char   chLongDir;//'E'/'W';
	double dfHgt;//�߳�
	char   chHgtUnit;//�̵߳�λ:m
}WAAInfo, *lpWAAInfo;

//ͨ����Ϣ�������� : CUNPACK_TXR 
typedef struct tagTXRInfo
{
	unsigned char byInfoType;//��Ϣ���:1~5��1:��ͨͨ�ţ�2:�ؿ�ͨ�ţ�3:ͨ��ͨ��,4:�����´�����Ĳ�ѯ��õ�ͨ��,5:�����ŷ���ַ��ѯ��õ�ͨ�š�
	int  nLocalID;		//���ŷ���ַ
	int  nDestionID;	//���ŷ���ַ
	unsigned char byTeleFormat;//������ʽ:0-���֣�1-���룬2-��ϴ���
	unsigned char bySendHour;  //����ʱ��(ʱ)��byInfoType == 1��2��3ʱ������ʱ����Ч��byInfoType ==4��5ʱ,Ϊ����ѯ��ͨ�ŵ��������Ŀ���ϵͳ��¼�ķ���ʱ�䡣
	unsigned char bySendMinute;//����ʱ��(��)��
	char chContent[1024];//ͨ�����ݡ�C������byTeleFormat == 2ʱ��������������ĸ�̶�ΪA4.
	int  nContentLen;
	unsigned char byCRC; //CRCУ�飬0-��ȷ 1-����

	bd_evt_type_t EventType;	// �¼����͡�   1���͵����澯 2���ƻ��澯	3����������	4��������ͣ����	5: ����ָ�����
	char  time[15];
	char  digest[33];
	double dfLong;
	double dfLat;
	float  fEarthHgt;			// ��ظ�.
}TXRInfo, *lpTXRInfo;

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
	unsigned char   byPrecisionInd; //��λ����ָʾ.0:һ��20�׾���;1:����100�׾���
	int   bUrgent;        //�Ƿ����
	int   bMultiValue;    //�Ƿ��ֵ��
	int   bGaoKong;       //�߿�ָʾ

}DWRInfo, *lpDWRInfo;


#endif