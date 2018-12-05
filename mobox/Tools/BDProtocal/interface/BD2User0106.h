#if !defined(BD2_USER_H)
#define BD2_USER_H

#include "nscHead.h"


#define BD2_UFUN_USER_START 90000	// ������ʼ��

// ����Դ����
enum STYPE
{
	STYPE_BD,	//�������ŵ�����λϵͳ(BD-2)
	STYPE_GP,	//ȫ��λϵͳ(GPS)
	STYPE_GN,	//ȫ�򵼺�����ϵͳ(GNSS)
	STYPE_GL,	//GLONASSϵͳ
	STYPE_GA,	//٤����ϵͳ
	STYPE_CC,	//�����ϵͳ
};

#define __SUCCESS  1
#define __FAILED   0

// ���ڣ�D[3]=��(YY)-��(MM)-��(DD)
// ʱ�䣺T[4]=ʱ-��-��-1/100��
// ����: L[4]=��-��-��-1/10��
// γ��: B[4]=��-��-��-1/10��
// GGA���ر���,��ΪҪ��ȷ��0.0001��
// ����: L[5]=��-��-��-1/10��-1/1000��
// γ��: B[5]=��-��-��-1/10��-1/1000��

//----------------------------------------------------------------------------------------------
// ���������
//----------------------------------------------------------------------------------------------

enum BD2_COMM_UFUN_SET
{	
	UFUN_SET_RMO=BD2_UFUN_USER_START,		// �������
	UFUN_SET_APL,							// ����Ա�������
	UFUN_SET_MSS,							// ģʽ����
	
	UFUN_SET_DWA,							// ��λ����,����:UFUN_GET_DWR
	UFUN_SET_TXA,							// ͨ������,����:UFUN_GET_TXR
	UFUN_SET_CXA,							// ��ѯ����,����:UFUN_GET_DWR,UFUN_GET_TXR,UFUN_GET_HZR
	
	UFUN_SET_WAA,							// λ�ñ���1(���ķ�ʽ)
	UFUN_SET_WBA,							// λ�ñ���2(��λ��ʽ)
	
	UFUN_SET_KLS,							// ����ʶ������(ָ�ӻ�������)
	UFUN_SET_KLT,							// ����ʶ��Ӧ��(������ָ�ӻ�)

	UFUN_SET_ICA,							// �û������,����:UFUN_GET_ICZ,UFUN_GET_ICI
	UFUN_SET_ZHS,							// �����Ի�
	UFUN_SET_JMS,							// ��Ĭ����

	UFUN_SET_GXM,							// ��������/��ȡ
	UFUN_SET_BSS,							// ��������

	UFUN_SET_RPQ,                           // λ�ñ�������
	UFUN_SET_TSQ,				            // ̬������
	UFUN_SET_FTX,                           // ����ͨ��
	UFUN_SET_WZB,                           // λ�ñ���
	UFUN_SET_SEC                            //1pps���������
};


#define WZBG_TYPE_BJZ54		0xA0
#define WZBG_TYPE_WGS84		0xA1
#define WZBG_TYPE_CGS2K		WZBG_TYPE_WGS84

#pragma pack(push)
#pragma pack(4)
//----------------------------------------------------------------------------------------------
// ��������Ĳ����ṹ
//----------------------------------------------------------------------------------------------
//һ����չЭ��ṹ��,λ�ñ��档

//λ�ñ�������A0
//750...
typedef struct tagLSET_WZBG
{
	UINT32   srcID;         // �����û���ַ
	UINT32   PeerID;        // �Է���ַ
	UINT8    PosType;       // ��λ����ϵ����(WZBG_TYPE_xxxx)
	UINT8    T[4];			// ��λʱ��(ʱ/��/��/�ٷ�֮һ��)
	UINT8	 L[4];			// ��λ����(��/��/��/ʮ��֮һ��)
	UINT8    B[4];			// ��λγ��(��/��/��/ʮ��֮һ��)
	int		 H;				// �߳�(��λ��)
	int      Segma;			// �߳��쳣(��λ��)
	// ȱ�߳�ָʾ��Ĭ��Ϊ��ͨ
}LSET_WZBG;

typedef LSET_WZBG  USET_WZBG;
typedef LSET_WZBG  *lpUSET_WZBG;
typedef LSET_WZBG  UGET_WZBG;
typedef LSET_WZBG  *lpUGET_WZBG;
typedef  USET_WZBG  UGET_WZBG;
//һ����չЭ��ṹ��,���ɱ��ġ�
// ���ɵ���A4
//750..
typedef struct tagLSET_ZYDW 
{
	UINT32   srcID;			// �����û�ID
	UINT32	 PeerID;		// �Է���ַ
	UINT8	 nLength;		// ���ĳ��ȣ��ֽ�
	char	 Msg[210];		// ����
}LSET_ZYDW;

typedef LSET_ZYDW  USET_ZYBW;
typedef LSET_ZYDW  USET_ZYDW;
typedef LSET_ZYDW  UGET_ZYBW;
typedef LSET_ZYDW  UGET_ZYDW;

//һ����չЭ��ṹ��,λ�ñ������󡣡�
//Ҫ��λ�ñ���A2
 //750...
typedef struct tagLSET_YQBG
{   
	UINT32   srcID;			// �ܿ��û�ID,һ��û��,����
	UINT32   PeerID;		// �շ�
	UINT8    Freq;			// Ƶ��(��)
	UINT8    Flag;			// �����־��0��Ӧ/1����Ӧ��	
	UINT8	 SubCount;		// �����û��б��еĸ���
	UINT32	 SubID[22];		// �����û��б�
}LSET_YQBG;

typedef LSET_YQBG  USET_RPQ;
typedef LSET_YQBG  *lpUSET_RPQ;
typedef LSET_YQBG  UGET_RPQ;
typedef LSET_YQBG  *lpUGET_RPQ;

typedef USET_RPQ  UGET_RPQ;

 //̬������A3
typedef struct tagLSET_TSSJ
{
	UINT32   srcID;				// ID
	UINT32	 PeerID;			// �Է���ַ
	UINT8	 UserCount;
	struct 
	{
		UINT32 UserID;			// �û�ID
		UINT32 T;				// ʱ�䣬��(��+ʱ+��+��)
		float L;				// ���ȣ���
		float B;				// γ�ȣ���
	} PosData[60];//�������в��ܳ�����ͬ��ID�ţ�ȷ����������ݾ�������󲻻ᳬ��ͨ��������
} LSET_TSSJ;
typedef LSET_TSSJ USET_TSQ;
typedef LSET_TSSJ *lpUSET_TSQ;
typedef LSET_TSSJ UGET_TSQ;
typedef LSET_TSSJ *lpUGET_TSQ;


// �������
typedef struct tagUSET_QQSC
{   
	tagUSET_QQSC(){memset(this, 0, sizeof(tagUSET_QQSC));};
	STYPE	src;            //
	UINT8	Mode;			// ģʽ��1=�ر�ָ����䣬2=��ָ����䣬3=ȫ�أ�4=ȫ��
	UINT16	Freq;			// Ƶ��(��λms)��ģʽ2ʱʹ�ã�
	char	Cmd[4];			// �������3���ַ�,���������Ϣ�ṹUGET_XXX�е�XXX��
}USET_QQSC,USET_RMO;

// ��λ����
// ֻ���ܽ��յ������������ź�(��������������ֱ����Բ�ͬ������)���ܽ��ж�λ.
// ��߷�ʽӰ�춨λ�Ľ���;���

//������߷�ʽ��Ϊ:
//    �в��: ��ظ�:Ϊ���ߴ��Ĵ�ظ߳�,��λ��; 
//    �޲��: ���߸�:Ϊ���߾������ĸ߶ȣ���λΪ0.1�ף�
//    ���1:  ���߸�:Ϊ���߾������ĸ߶ȣ���λΪ0.1�ף�
//			  ��ѹ:  ���ߴ��Ĵ���ѹ,��λ0.1Hp,����û����ڲ��Դ���ѹ�ǣ��������0��;
//			  �¶�:  ��ʱ������,��λ0.1��,����û����ڲ��Դ��¶ȼƣ��������0��;
//    ���2:  ��ظ�,Ϊ�û�������ѹ������λ�õ�������,��λ�ף�
//            ���߸�:���߾����û�������ѹ�ǵĸ߶ȣ���λΪ0.1��
//			  ��ѹ:  ���ߴ��Ĵ���ѹ,��λ0.1Hp,����û����ڲ��Դ���ѹ�ǣ��������0��;
//			  �¶�:  ��ʱ������,��λ0.1��;

typedef struct tagUSET_DWSQ
{
	tagUSET_DWSQ(){memset(this, 0, sizeof(tagUSET_DWSQ));};
	UINT32 UserID;				// ��λ���û���ID
	UINT8  Flag;				// ��Ϣ��־,�μ����������
	UINT16 Freq;				// ��վƵ��(��λ��),Ƶ��Ϊ0ʱ,ֻ��λһ��
	INT32  Altitude;			// ��ظߣ���λ�ף�
	UINT32 AntHeight;			// ������Ե���ĸߣ���λ0.1�ף�
	UINT32 Pressure;			// ��ѹ����λ0.1Hp��
	INT16  Temperature;			// �¶ȣ���λ0.1�ȣ�
} USET_DWSQ,USET_DWA;

#define DWSQ_FLAG_URGE			0x10	// ������λ,Ҫռ��2��Ƶ��
#define DWSQ_FLAG_COMM			0x00	// ��ͨ��λ

#define DWSQ_FLAG_HEIGHT_HAVE	0x00	// �в��
#define DWSQ_FLAG_HEIGHT_NO		0x01	// �޲��
#define DWSQ_FLAG_HEIGHT_1		0x02	// ���1
#define DWSQ_FLAG_HEIGHT_2		0x03	// ���2

#define TX_FLAG_CHN	0
#define TX_FLAG_BCD	1
#define TX_FLAG_MIX	2	// ��չͨ��Э��֧��

// ͨ������
typedef struct tagUSET_TXSQ
{
	tagUSET_TXSQ(){memset(this, 0, sizeof(tagUSET_TXSQ));};
	UINT8	Flag;				// ��Ϣ���0=���֣�1=���룬2=���(��չͨ��֧��)3:zhitong 
	UINT8	Fast;				// ****�Ƿ��ؿ�0=�ؿ죬1=��ͨ
	UINT32  DestID;				// ���ն�ID,���ʹ�õ���ָ�ӿ�,�Ҹõ�ַΪͨ����ַ,��Ϊ�㲥,��Ͻ�û������յ�
	UINT16  Len;				// ���ĳ���(Byte)
	char	Msg[421];			// ��������0xF1;
}USET_TXSQ,USET_TXA;

// ��ѯ����
// �û��Ķ�λ���,�շ��ı���,�յ���Ļ�ִ,��������վ����һ��ʱ��,�û�����ͨ����������в�ѯ.
//
// ��λ��ѯ:��ѯ�Լ�������Ͻ�û�����������ζ�λ���(����ǰ��������,��û���յ�)
// ��"����"��ʽ,���Բ�ѯ�����û������Լ���δ�յ������5�α��ģ�ÿ��ѯһ�Σ�����1�� 
// ��"���͵�ַ"��ʽ,���Բ�ѯָ���û������Լ���δ�յ��ı���                           
// ��"��ִ"��ʽ,���Բ�ѯָ���Լ�����ָ���û����ĺ�,�Է���������վ��"�յ���ִ"
typedef struct tagUSET_CXSQ
{	
	tagUSET_CXSQ(){memset(this, 0, sizeof(tagUSET_CXSQ));};
	UINT32  UserID;				// �����û�ID
	UINT8   Kind;				// ��ѯ���(0=��λ,1=ͨ��)	
	union 
	{
		UINT8   Count;			// ��λ��ѯʱΪ"��λ����":01=1��, 02=2��,       03=3��       
		UINT8   Style;			// ͨ�Ų�ѯʱΪ"��ѯ��ʽ":01=����,02=��������ַ,03=��ִ    
	};
} USET_CXSQ,USET_CXA;

// λ�ñ���1(���ķ�ʽ��
// ��������ʱ,Ӧ�ò���д�ýṹ,������ר��λ�ñ������(����ͨ����չ)����
typedef struct tagUSET_WZBG1
{
	tagUSET_WZBG1(){memset(this, 0, sizeof(tagUSET_WZBG1));};
	UINT32  SendID;				// ����ID
	UINT32  DestID;				// �շ�ID
	UINT8   T[4];				// ����ʱ��
	UINT8   LSign;				// ���ȷ��ţ�0=������1=������
	UINT8   L[4];				// ����
	UINT8   BSign;				// γ�ȷ��ţ�0=��γ��1=��γ��
	UINT8   B[4];				// γ��
	float   Height;				// �̣߳��ף�
} USET_WZBG1,USET_WAA;

// λ�ñ���2(��λ��ʽ--�����޸߳�/�����߸ߡ���λ��
// ��������ʱ��Ͷ�λ����,��վ����,���������͸��շ�
typedef struct tagUSET_WZBG2
{
	tagUSET_WZBG2(){memset(this, 0, sizeof(tagUSET_WZBG2));};
	UINT32  DestID;				// �շ�ID
	UINT8   Freq;				// ����Ƶ��
	float   AntHeight;			// ���߸�(�ף�
} USET_WZBG2, USET_WBA;

// ����ʶ��(�ϼ�����ʶ������)
typedef struct tagUSET_KLML
{
	tagUSET_KLML(){memset(this, 0, sizeof(tagUSET_KLML));};
	UINT32	SubID;				// �¼���ַ
	char	Response;			// Y=Ӧ��N=��Ӧ��
} USET_KLML, USET_KLS;
typedef USET_KLS  UGET_KLS;

// ����ʶ��(�¼�Ӧ���ϼ���
typedef struct tagUSET_KLYD
{   
	tagUSET_KLYD(){memset(this, 0, sizeof(tagUSET_KLYD));};
	UINT32	PeerID;				// �Է���ַ
	UINT8	Flag;				// ��Ϣ���,0=���֣�1=���룬2=���(��չͨ��֧��)
	UINT8	Len;				// ����(�ֽ�)
	char    Msg[22];			// ����
} USET_KLYD, USET_KLT;
// ����ʶ��Ӧ��(ָ�ӻ������¼���Ӧ��)
typedef USET_KLT  UGET_KLT;

// �û������(��ȡ�û���Ϣ)
typedef struct tagUSET_ICJC
{
	tagUSET_ICJC(){memset(this, 0, sizeof(tagUSET_ICJC));};
	UINT8    FrameNo;			// ֡��(0=����,0~127=������Ͻ�û�,128~255=��ӷ����û�)
	UINT8	 Num;				// ���0-3
} USET_ICJC,USET_ICA;

// �豸�������ã������û����ĳ�ʼ��
typedef struct tagUSET_GLXX
{
	tagUSET_GLXX(){memset(this, 0, sizeof(tagUSET_GLXX));};
	UINT8   Mode;				// ���ͣ�1=����, 2=��ȡ
	char	ManageInfo[64];		// ������Ϣ
} USET_GLXX,USET_GXM;

// ��Ĭ����
typedef struct tagUSET_JMSZ
{
	tagUSET_JMSZ(){memset(this, 0, sizeof(tagUSET_JMSZ));};
	UINT8   Silience;			// 0=�����Ĭ,1=��Ĭ1(��ֹ��ִ����վ),2=��Ĭ2(��ֹ��ִ��������վ��ͨ��/��λ/��ʱ��)
} USET_JMSZ,USET_JMS;

// �����Ի�
// ֻ�������ⳡ����ʹ��.һ�������˸�����,������ɾ�����е�����������û�������,�Ժ�����ʹ��
typedef struct tagUSET_JJZH
{
	tagUSET_JJZH(){memset(this, 0, sizeof(tagUSET_JJZH));};
	UINT8   Reserved[4];		// 0xAA5555AA
} USET_JJZH,USET_ZHS;

// ��������
typedef struct tagUSET_APL
{   
	tagUSET_APL(){memset(this, 0, sizeof(tagUSET_APL));};
	STYPE	src;			
	UINT8	T[4];			// *UTCʱ��
	float	LevelH;			// ˮƽ��������
	float	LevelV;			// ��ֱ��������
	float	Level3D;		// �ռ䱣������
}USET_APL;
typedef USET_APL  UGET_APL ;

// ��λģʽ
// �ڹ���ģʽ�£���λģʽ������1=BD-RNSS��Ƶ��λ��2=˫Ƶ��λ��3=RDSS��λ��4=GPS��λ��5=���ݶ�λ
typedef struct tagUSET_MSS
{
	tagUSET_MSS(){memset(this, 0, sizeof(tagUSET_MSS));};
	STYPE	src;			
	char	WorkMode;		// ����ģʽ��'C'=����ģʽ��'Z'=����ģʽ
	UINT8	PosMode;		// ��λģʽ: 0~9
	UINT8   Freq[3];		// Ƶ��(0~5��{"","B1","B2","B3","L1","G1"}
	char	IQ[3];			// ֧·��C=C�룬P=P�룬A=C/P����)
} USET_MSS;
enum{MSS_B1_NONE = 0,MSS_B1 = 1,MSS_B2 = 2,MSS_B3 = 3,MSS_L1 = 4,MSS_G1 = 5};

// ���ò��������������û��豸����Ӧ������ʱ���
typedef struct tagSET_BSS
{
	tagSET_BSS(){memset(this, 0, sizeof(tagSET_BSS));};
	UINT8  nResponseBeam;// 1.��Ӧ���������ڲ��������źŵ��û����ܲ�����ȡֵ��Χ0-10
	UINT8   nTimeDifBeam;// 2.ʱ�����ָ�û��豸˫ͨ�����ܹ������ǽ���ʱ������ķ���Ӧ������ȡֵ��Χ0-10
} USET_BSS;

// ϵͳ�Լ�
typedef struct tagSET_XTZJ
{
	tagSET_XTZJ(){memset(this, 0, sizeof(tagSET_XTZJ));};
	UINT32  srcID;
	UINT8   Freq;                // Ƶ��
} USET_XTZJ;

#define MSS_FREQ_NONE	0
#define MSS_FREQ_B1		1
#define MSS_FREQ_B2		2
#define MSS_FREQ_B3		3
#define MSS_FREQ_L1		4	// GPS L1
#define MSS_FREQ_G1     5	// GLONASS L1

//----------------------------------------------------------------------------------------------
// ������Ϣ���ܳ���
//----------------------------------------------------------------------------------------------
enum BD2_COMM_UFUN_GET
{
	UFUN_GET_ZDA=BD2_UFUN_USER_START,	// ��ʱ��Ϣ
	UFUN_GET_ZTI,						// ����״̬��Ϣ
	UFUN_GET_APL,						// ��������

	//RNSS
	UFUN_GET_GGA,					 	// ��λ��Ϣ
	UFUN_GET_GSA,						// ��ǰ������Ϣ
	UFUN_GET_GSV,						// �ɼ�������Ϣ
	UFUN_GET_RMC,						// �Ƽ���λ��Ϣ
	UFUN_GET_GLL,						// ��λ������Ϣ
	UFUN_GET_VTG,						// �����ٶ���Ϣ
	UFUN_GET_ICM,						// ����ģ����Ч��
	UFUN_GET_PMU,						// PRMģ����Ч��
    
	//RDSS
	UFUN_GET_DWR,						// ��λ��Ϣ
	UFUN_GET_TXR,						// ͨ����Ϣ
	UFUN_GET_HZR,						// ͨ�Ż�ִ
	UFUN_GET_DWXJ,						// ���������û���λ��Ϣ
	UFUN_GET_TXXJ,						// ���������û�ͨ����Ϣ
	UFUN_GET_WAA,						// λ�ñ���1

	UFUN_GET_ICI,						// �����û�����Ϣ
	UFUN_GET_ICZ,						// �����û�����Ϣ

	UFUN_GET_GXM,						// ������Ϣ
	UFUN_GET_BSI,						// ����״��
	UFUN_GET_FKI,						// ���䷴��
	
	UFUN_GET_KLT,						// ����ʶ���¼�Ӧ��
	UFUN_GET_BID,						// ��ȡ��λģʽ
	UFUN_GET_ZHR,						// �Ի�ȷ����Ϣ
	UFUN_GET_DTY,                       // �豸��Ϣ
	UFUN_GET_RPQ,                       // λ�ñ���������Ϣ
	UFUN_GET_TSQ,                       // ̬�Ʒ���
	UFUN_GET_FTX,                       // ����ͨ��
	UFUN_GET_WZB,                       // λ�ñ���
};

// ��ʱ��Ϣ
typedef struct tagUGET_ZDA
{
	tagUGET_ZDA(){memset(this, 0, sizeof(tagUGET_ZDA));};
	STYPE	src;			
	UINT8   Mode;			// *��ʱģʽ��1=RDSS,2=RNSS
	UINT8   T[4];			// UTCʱ��
	UINT8	D[3];			// UTC����
	INT8	LocalH;			// ����ʱ-HH(-13~+13,��=��������=������
	UINT8	LocalM;			// ����ʱ-MM(00~59���ӣ�
	UINT8	CorrectT[4];	// *ʱ������ֵʱ��
	float	CorrectData;	// *����ֵ	
	UINT8	Precise;		// *����ָʾ��0=δ֪��1=0~10ns, 2=10~20ns,3��20ns��
	char	Status;			// *��������״̬��Y=������N=ʧ��)
}UGET_ZDA;

// ״̬��Ϣ
typedef struct tagUGET_ZTI
{
	tagUGET_ZTI(){memset(this, 0, sizeof(tagUGET_ZTI));};
	STYPE	src;			
	UINT8   PRM;			// PRMģ�飨0=������1=���ϣ�
	UINT8   Encrypt;		// �ӽ���ģ�飨0=������1=���ϣ�
	UINT8	Annatena;		// ���ߣ�0=������1=���ϣ�
	UINT8	Channels;		// ͨ����0=������1=���ϣ�
	UINT8	OutPower;		// ����磨0=��أ�1=��磩
	UINT8	Recharged;		// ���ָʾ��0=��磬1=�ǳ��
	float	RestPower;		// ʣ�����%
	UINT8	RestHM[2];		// ���õ����Ŀ���ʱ�䣨[0]=ʱ��[1]=�֣�
}UGET_ZTI;

//----------------------------------------------------------------------------------------------
// RNSS-��������
//----------------------------------------------------------------------------------------------

// GGA,��λ��Ϣ
// ״ָ̬ʾ��
//   GP��λʱ��0=�����ã�1=SPS��λ��Ч��2=��ֶ�λ��Ч��3=PPS��λ��Ч��4=RTK��λ��Ч���������⣬5=RTK��λ��Ч���и����⣬6=��������7=�ֶ�ģʽ��8=ģ����ģʽ
//   BD��λʱ��0=�����ã�1=�޲�ֶ�λ��Ч��2=��ֶ�λ��Ч��3=˫Ƶ��λ��Ч
//   GN��λʱ��0=�����ã�1=���ݶ�λ��Ч
typedef struct tagUGET_GGA
{
	tagUGET_GGA(){memset(this, 0, sizeof(tagUGET_GGA));};
	STYPE	src;					// ��Դ
	UINT8   T[4];					// ʱ��
	UINT8   LSign;					// ���ȷ��ţ�0=������1=������
	UINT8   L[5];					// ����
	UINT8   BSign;					// γ�ȷ��ţ�0=��γ��1=��γ��
	UINT8   B[5];					// γ��
	float   Height;					// �̣߳��ף�
	float   Segma;					// *�߳��쳣(��λ��,�����CGC2000)
	UINT8   Flag;					// *״ָ̬ʾ(0~9��
	UINT8   SateCount;				// ʹ�õ�������
	float   HDOP;					// ˮƽ��������
	float   VDOP;					// *��ֱ��������
	UINT8   DiffTime;				// �����������
	UINT16  DiffID;					// ���ID��, 0000-1023
} UGET_GGA;

// GSA,��ǰ������Ϣ��������������
// ���ݶ�λʱ��ÿ������������һ��������
typedef struct tagUGET_GSA
{
	tagUGET_GSA(){memset(this, 0, sizeof(tagUGET_GSA));};
	STYPE	src;					// ��Դ
	char    WorkMode;				// ����ģʽ, A=�Զ�, M=�˹�
	UINT8   PosMode;				// ��λģʽ, 1=�޷���λ, 2=��ά��λ, 3=��ά��λ
	UINT8   Satellite[12];			// ���� #1---#12
	float   PDOP;					// λ�þ�������
	float   HDOP;					// ˮƽ��������
	float   VDOP;					// ��ֱ��������
	float   TDOP;					// *ʱ�侫������
}UGET_GSA;

// GSV,�ɼ�������Ϣ
typedef struct tagSAT_INFO
{
	tagSAT_INFO(){memset(this, 0, sizeof(tagSAT_INFO));};
	UINT8	SatNo;				// ����
	float	Elevation;			// ���ǵ�����0-90
	float	Azimuth;			// ���ǵķ�λ��0-359
	float	fSNR;				// �����
} SAT_INFO;

typedef struct tagUGET_GSV
{
	tagUGET_GSV(){memset(this, 0, sizeof(tagUGET_GSV));};
	STYPE	src;					// ��Դ
	UINT8   TotalInfo;				// ��Ϣ����, 1-3
	UINT8   InfoID;					// ��ǰ��Ϣ�� 1-3
	UINT8   SatCount;				// ��������(����GSV�����е���������)
	SAT_INFO Satellite[4];
}UGET_GSV;

// RMC,�Ƽ���λ��Ϣ
typedef struct tagUGET_RMC
{
	tagUGET_RMC(){memset(this, 0, sizeof(tagUGET_RMC));};
	STYPE	 src;					// ��Դ
	UINT8	 D[3];                  // UTC����
	UINT8	 T[4];                  // UTCʱ��
	UINT8	 LSign;				    // ���ȷ��ţ�0=������1=������
	UINT8	 L[4];				    // ����
	UINT8	 BSign;				    // γ�ȷ��ţ�0=��γ��1=��γ��
	UINT8	 B[4];				    // γ��
	float    Speed;                 // �����ٶ�(��/��)
	float    FollowArc;             // ���溽��������汱��˳ʱ��Ϊ����
	float    MagneticVariation;     // ��ƫ��(��=������=����
	char     Mode;                  // *ģʽָʾ(A=������λ��D=��ֶ�λ��E=���㣬N=��Ч)
}UGET_RMC;

// GLL����λ������Ϣ
typedef struct tagUGET_GLL
{
	tagUGET_GLL(){memset(this, 0, sizeof(tagUGET_GLL));};
	STYPE	src;					// ��Դ
	UINT8   L[4];					// ����
	UINT8	LSign;				    // ���ȷ��ţ�0=������1=������
	UINT8   B[4];                   // γ��
	UINT8	BSign;				    // γ�ȷ��ţ�0=��γ��1=��γ��
	UINT8   T[4];                   // UTCʱ��
	char    Mode;					// *ģʽָʾ��A=�Զ�ģʽ��D=��֣�E=���㣬M=�ֶ����룬S=ģ����,N=��Ч��
}UGET_GLL;

// VDT,�����ٶ���Ϣ
typedef struct tagUGET_VTG
{
	tagUGET_VTG(){memset(this, 0, sizeof(tagUGET_VTG));};
	STYPE	  src;					// ��Դ
	float     RNC;					// ���汱Ϊ�ο���׼�ĵ��溽��000~359�ȣ�
	float     MNC;					// �Դű�Ϊ�ο���׼�ĵ��溽��000~359�ȣ�
	float     KTS;					// �����ٶȣ�000.0~999.9�ڣ�
	float     KPH;					// �����ٶȣ�0000.0~1851.8����/Сʱ)
	char      Mode;					// *ģʽָʾ��A=������λ��D=��ֶ�λ��E=���㣬M=�ֶ���S=ģ������N=��Ч��
}UGET_VTG;

// haige phb 2.1Э�鿪ʼ������ʼ����
// ����ģ�����Ч��
typedef struct tagUGET_ICM
{
	tagUGET_ICM(){memset(this, 0, sizeof(tagUGET_ICM));};
	STYPE	src;					// ��Դ
	UINT8   D1[3];					// ��ʼ����
	UINT8   D2[3];					// ��������
}UGET_ICM;

// PRMģ�����Ч��
typedef struct tagUGET_PMU
{
	tagUGET_PMU(){memset(this, 0, sizeof(tagUGET_PMU));};
	STYPE	src;					// ��Դ
	UINT8	Freq;					// B1/B2/B3
	UINT8   D1[3];					// ��ʼ����
	UINT8   D2[3];					// ��������
}UGET_PMU;

//----------------------------------------------------------------------------------------------
// RDSS-��������
//----------------------------------------------------------------------------------------------

// ��λ��Ϣ(������λ����λ��ѯ��λ�ñ���2������������λ��
// ������λʱSendID=0
typedef struct tagUGET_DWXX
{
	tagUGET_DWXX(){memset(this, 0, sizeof(tagUGET_DWXX));};
	UINT32  SendID;			// ����ID
	UINT8   Kind;			// ��Ϣ���1=��λ�����2=��ѯ�����3=λ�ñ���(��ʽ2)
	UINT8	Flag;			// ��Ϣ��־ ��λ����ָʾ 0һ����1����
	UINT8   T[4];			// UTCʱ��
	UINT8   LSign;			// ���ȷ��ţ�0=������1=������
	UINT8	L[4];			// ��λ����
	UINT8   BSign;			// γ�ȷ��ţ�0=��γ��1=��γ��
	UINT8   B[4];			// ��λγ��
	float	Height;			// �߳�(��λ��)
	float   Segma;			// �߳��쳣(��λ��,�Ը߿��û�,���ֶ�Ϊ0)
} UGET_DWXX,UGET_DWR;

#define DWXX_FLAG_PRICISE2	0x08	// Ϊ��������(100��),����Ϊһ������(20��),
#define DWXX_FLAG_URGED		0x04	// Ϊ������λ
#define DWXX_FLAG_MULTI_POS	0x02	// Ϊ��ֵ��
#define DWXX_FLAG_AIR		0x01	// Ϊ�߿ն�λ

#define DWXX_KIND_POS	1			// ��λ���
#define DWXX_KIND_QRY	2			// ��ѯ���
#define DWXX_KIND_RPT	3			// λ�ñ���2

#define UGET_DWXJ UGET_DWR	// ����������λ

// ͨ����Ϣ���������գ���ѯͨ�ţ���������ͨ�ţ�
typedef struct tagUGET_TXXX
{
	tagUGET_TXXX(){memset(this, 0, sizeof(tagUGET_TXXX));};
	UINT32	SendID;			// ����ID
	UINT32	DestID;			// �շ�ID
	UINT8	Kind;			// ��Ϣ���:1=��ͨ���ģ�2=�ؿ죬3=ͨ����4=�����´����ѯ���ģ�5=��������ѯ����
	UINT8	Flag;			// ��Ϣ���0=���֣�1=BCD�룬2=���(��չ��֧��)
	UINT8	H;				// ����ʱ�䣺Сʱ,��ͨ�Ų�ѯ�Ľ����Ч
	UINT8	M;				// ����ʱ�䣺��,��ͨ�Ų�ѯ�Ľ����Ч
	UINT16	Len;			// ���ȣ��ֽ�)
	char	Msg[421];		// ����(ע��,û���ڴ����'\0')
	UINT8	CRC;
}UGET_TXXX,UGET_TXR;

#define TXXX_KIND_GNL		1	// ��ͨ
#define TXXX_KIND_FST		2	// �ؿ�
#define TXXX_KIND_BRD		3	// ͨ��
#define TXXX_KIND_QRY_LST	4	// �����´����ѯ����
#define TXXX_KIND_QRY_SND	5	// ��������ѯ����

typedef UGET_TXR  UGET_TXXJ ;	// ��������ͨ��

// ͨ�Ż�ִ
// ��ִ��ѯ����ķ��ؽ��
typedef struct tagUGET_TXHZ
{
	tagUGET_TXHZ(){memset(this, 0, sizeof(tagUGET_TXHZ));};
	UINT32 DestID;			// Ŀ��ID
	UINT8  Count;			// ��ִ����,���5��
	struct 
	{
		UINT8 SendHour;		// ����ʱ
		UINT8 SendMin;		// ���ͷ�
		UINT8 AnswerHour;	// Ӧ��ʱ
		UINT8 AnswerMin;	// Ӧ���
	} Times[5];
}UGET_TXHZ,UGET_HZR;

// λ�ñ���1(���ķ�ʽ��
typedef USET_WAA  UGET_WZBG1 ;
typedef USET_WAA  UGET_WAA   ;

// �����û�����Ϣ
typedef struct tagUGET_ICBJ
{
	tagUGET_ICBJ(){memset(this, 0, sizeof(tagUGET_ICBJ));};
	UINT32	UserID;			  // �����û�ID
	UINT32	SerialNo;		  // ���к�
	UINT32  BroadcastID;	  // ͨ��ID
	UINT8   Flag;			  // �û�����,�����������
	UINT16  Freq;			  // ����Ƶ��
	UINT8   Grade;			  // ͨ�ŵȼ�(1~4)
	UINT8   Encrypt;		  // ���ܱ�־(0=��,1=��)
	UINT16  UserTotal;		  // �����û�����(ĳЩ�豸��Ч)
}UGET_ICBJ, UGET_ICI;

#define ICBJ_FLAG_USER  0x03		// ����ͨ�û���,�������ø������ж��Ƿ�Ϊָ�ӿ�
#define ICBJ_FLAG_IDREG 0x04		// ��Ҫ�����֤

// �¼��û���Ϣ
// ÿ����෵��40��
typedef struct tagUGET_ICXJ
{
	tagUGET_ICXJ(){memset(this, 0, sizeof(tagUGET_ICXJ));};
	UINT8     UserCount;    // ��֡�û���
	UINT32    UserIDs[40];	// ��Ͻ�û�
} UGET_ICXJ;

// ����״����Ϣ
typedef struct tagUGET_GLZK
{
	tagUGET_GLZK(){memset(this, 0, sizeof(tagUGET_GLZK));};
	UINT8	MainBeam;		// ������(0=û������,0xff=δ֪)
	UINT8	SlaveBeam;		// �Ӳ���(0=û������,0xff=δ֪)
	UINT8	Beams[10];		// ÿ��ͨ�����ź�ǿ��(0~4)
} UGET_GLZK,UGET_BSI;

// ������Ϣ
typedef struct tagUGET_GXM
{
	tagUGET_GXM(){memset(this, 0, sizeof(tagUGET_GXM));};
	UINT8	ManageInfo[64+1];	// ������Ϣ
}UGET_GLXX,UGET_GXM;

// ���䷴����Ϣ
typedef struct tagUGET_FKXX
{
	tagUGET_FKXX(){memset(this, 0, sizeof(tagUGET_FKXX));};
	char	Cmd[4];			// ������(�����ַ�)
	UINT8	Result;			// ���(0=�ɹ���1=ʧ�ܣ�
	UINT8	Freq;			// Ƶ�ȣ�0=��ȷ��1=��Ƶ
	UINT8	Launch;			// ����ָʾ:0=�������ƽ����1=�������ƣ�2=�������㣬3=���޾�Ĭ
	UINT16	WaitTime;		// �ȴ�ʱ��(s)
} UGET_FKXX,UGET_FKI;

// ��λģʽ��GET_BID��
typedef struct tagGET_BID
{   
	tagGET_BID(){memset(this, 0, sizeof(tagGET_BID));};
	UINT8 nFrec; // 1=B1,2=B1B3,3=B3,4=B3L1G1,5=L1,6=G1,7=B1L1
	char  cCP;	 // 'C'=C�룬'P'=P�룬'A'=P��C��
} UGET_BID;

typedef struct tagGET_ZHR
{
	tagGET_ZHR(){memset(this, 0, sizeof(tagGET_ZHR));};
	char ZHInfo[16]; // "AA5555AA"�����Իٳɹ�
} UGET_ZHR;

// �豸��Ϣ(RMO�����ȡ)
typedef struct tagSET_DTY
{
	tagSET_DTY(){memset(this, 0, sizeof(tagSET_DTY));};
	char	Vendor[16];	    // ������Ϣ
	UINT8	Modal;          // �ͺ�        0:������  1:��׼��
} UGET_DTY;

//��Ӧ����� UFUN_SET_SEC
typedef struct tagSET_SEC
{
	int nMode;
}USET_SEC,*LPUSET_SEC;

#pragma pack(pop)

#endif
