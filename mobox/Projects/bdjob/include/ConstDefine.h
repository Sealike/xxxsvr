#ifndef  __CONSTDEFINE_H
#define  __CONSTDEFINE_H
//////////////////////////////////////////////////////////////////////////
//���ļ��������г������壬�����Ķ�����ͬ������DllInterface.h
//////////////////////////////////////////////////////////////////////////
#define PROTOCAL_EX_API     extern "C" __declspec(dllexport)
#define PROTOCAL_IM_API     extern "C" __declspec(dllimport)
#define PROTOCAL_EX_CLASS              __declspec(dllexport)
#define PROTOCAL_IM_CLASS              __declspec(dllimport)
#define DLLMAKEMFCCALL()    AFX_MANAGE_STATE(AfxGetStaticModuleState());


enum{
	CUNPACK_NULL,
	CUNPACK_BSI,//������Ϣ1
	CUNPACK_DWR,//��λ��Ϣ2
	CUNPACK_FKI,//������Ϣ3
	CUNPACK_ICI,//�ӽ���ģ����Ϣ4
	CUNPACK_ICZ,//ָ�ӻ������û���Ϣ5
	CUNPACK_KLS,//����ʶ��Ӧ��6
	CUNPACK_HZR,//��ִ��Ϣ7
	CUNPACK_TXR,//ͨ����Ϣ8
	CUNPACK_SBX,//�豸��ز���9
	CUNPACK_ZTI,//�û��豸״̬��Ϣ10
	CUNPACK_ZDA, //UTCʱ�䣬���ڣ��ͱ���ʱ������Ϣ11
	CUNPACK_LZM,//�豸��ֵ��Ϣ12
	CUNPACK_WAA,//λ�ñ���13
	CUNPACK_WBA,//λ�ñ���14
	CUNPACK_BGJG,//15δ�á�
	CUNPACK_GGA,//16RNSS��λ���
	CUNPACK_RMC,//17RNSS��λ������Ϣ
	CUNPACK_GSV,//18�ɼ�������Ϣ
	CUNPACK_PMU,//19PMU�����䣬PRMʱЧ������Ϣ��
	CUNPACK_ICM,//20
	CUNPACK_PKY,//21PRM����������̬��ע��
	CUNPACK_ALM,//22˫����䣬��������������
	CUNPACK_BSD,//23��ʾ����Ϣ
	CUNPACK_BSQ,//24��ʾ����Ϣ
	CUNPACK_BSX,//25��ʾ����Ϣ
	CUNPACK_ZTG,
	CUNPACK_GBS,//GNSS���ǹ��ϼ�⡣
	CUNPACK_GLL,
	CUNPACK_ECT,
	CUNPACK_KLT,
	CUNPACK_GSA,//���붨λ����
	CUNPACK_GXM,
	CUNPACK_GLZK,		//����״�� 01
	CUNPACK_DWXX,		//��λ��Ϣ 02
	CUNPACK_TXXX,		//ͨ����Ϣ 03
	CUNPACK_TXHZ,		//ͨ�Ż�ִ 04
	CUNPACK_KLSB,		//����ʶ�� 05
	CUNPACK_DHSC,		//������� 06
	CUNPACK_DSJG,		//��ʱ��� 07
	CUNPACK_SCSJ,		//ʱ������ 08 
	CUNPACK_ZBSC,		//������� 09
	CUNPACK_GBSC,		//�㲥��� 10
	CUNPACK_GYSC,		//������� 11 
	CUNPACK_ICXX,		//IC��Ϣ   12
	CUNPACK_ZHQR,		//�Ի�ȷ�� 13
	CUNPACK_ZJXX,		//�Լ���Ϣ 14
	CUNPACK_LZXX,		//��ֵ��Ϣ 15
	CUNPACK_SJXX,		//ʱ����Ϣ 16
	CUNPACK_BBXX,		//�汾��Ϣ 17 
	CUNPACK_XHXX,		//�����Ϣ 18
	CUNPACK_GLXX,		//������Ϣ 19
	CUNPACK_FKXX,		//������Ϣ 20
	CUNPACK_BSXX,		//��ʶ��Ϣ 21
	CUNPACK_HXXX,		//������Ϣ 22
	CUNPACK_HJSC,		//������� 23
	CUNPACK_ILXX,		//I·��Ϣ  24 
	CUNPACK_QLXX,		//Q·��Ϣ  25
	CUNPACK_RNCS,		//Q·��Ϣ  26
	CUNPACK_QUERYPOS,	//��λ��ѯ(�Ӷ�λ��Ϣ�н��)
	CUNPACK_QUERYCOMM,	//ͨ�Ų�ѯ(��ͨ����Ϣ�н��)
	//------------ͨ��Э����չ
	CUNPACK_MIXCOMMINFO,//�췢����ͨ����Ϣ(Э��0xA0)
	CUNPACK_POSREPORT,	//λ�ñ������(Э��0xA4)
	CUNPACK_XSD,//��ʾ������λ��Ϣ
	CUNPACK_XST,//��ʾ����ͨ����Ϣ
	CUNPACK_DHV,//��ʾ�ٶ���Ϣ
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
	 DEVICE_BDBSS =23,	  //RDSS ��������ָ��2.1
	 DEVICE_BDCXA =24,    //RDSS ��ѯָ��2.1
	 DEVICE_BDRMO =25,
	 DEVICE_BDPKY =26,
	 DEVICE_BDGXM =27,		//RDSS ����ע��ָ��
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
	 DEVICE_BDZHS =46,//�Ի�����
	 DEVICE_BDECS =47,
	 DEVICE_SATDEL=48,
	 DEVICE_MODPPS=49,
	 DEVICE_BDRNS =50,
	 DEVICE_BDRPQ =51,//λ�ñ�������
	 DEVICE_BDAPL =52,
	 DEVICE_SECPULSE = 53,
};

//typedef enum _USER_TYPE{
//	LOCALUSER=1,
//	SUBUSER,
//	OTHERUSER,
//}user_type;

#define MAXCHILDRENNUM	50
/* IQ֧·����������������� */
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
