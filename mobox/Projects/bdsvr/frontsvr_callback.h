#ifndef _FRONTSVR_CALLBACK__H__
#define _FRONTSVR_CALLBACK__H__

#include "instapp.h"
#include "p2p.h"

typedef struct _path_st
{
	int id;
	int taskid;
	char *  lat;
	char *  lon;
	char *  hgt;
	char *  mark;
}path_st,*path_t;

typedef struct _bdjob_json_st
{
	/*int tonumber;
	int totown;
	int tocountry;
	int togroup;
	int ifbroad;
	char *time;
	char *content;*/

	int id;				// ��������
	char * desc;		// ��������
	char * maker;		// �����ƶ���
	int taskid;			// �������
	path_st path[10];	// λ�õ㼯��
	int pathpointnum;	// path��λ�õ�ĸ���
	char * userid;		// �û�����
	char * maketime;	// �����ƶ�ʱ��
	char * taskname;	// ��������


}bdjob_json_st, *bdjob_json_t;




typedef enum
{
	inet2bd = 0,
	sms2bd
}which2bd_t;


typedef struct tagTXAInfo
{
	int nLocalId;   //�������������û���ID�ţ�������
	int  nDestId;   //�˴�ͨ�ŵ����ŷ���ַ��
	BYTE byCommCat; //ͨ�����0-�ؿ�ͨ�ţ�1-��ͨͨ�š�
	BYTE byCodeType;//���䷽ʽ��0-���֣�1-���룬2-��ϡ�
	BOOL ifcmd;   //����ʶ�� 0-ͨ�ţ�1-����ʶ��������
	BOOL ifAnswer; //�Ƿ�Ӧ�� ������ʶ��Ϊ0ʱ���Ƿ�Ӧ�������ȫ0  ��������
	char chContent[1024];//ͨ������.byCodeType ==0:ÿ������14bits,�Լ�������봫��;byCodeType==1,ÿ��bcd��ռ4bits���췢ʱ��ÿ���ֽ�ռ8bits
	int  nContentLen;    //ͨ�����ݵ��ֽ��������ĳ��ȣ�
	unsigned char byExpandType;		// ��չ���ͣ�0-ƽ̨�û���ʱ���ţ�1-�ֻ����뼴ʱ���ţ�2-ƽ̨�û�Ⱥ������
	char FromPhoneNum[12];	// ͨ�ŷ��ŷ��ֻ�����
	which2bd_t which2bd;
	//         byCodeType ==1:ÿ��������һ��ASCII��ʾ��        
	//         byCodeType ==2:������������ĸ�̶�ΪA4��     
}TXAInfo, *lpTXAInfo;

#ifdef __cplusplus
extern "C"
{
#endif

	void  frontsvr_exp_cb(int msgid, void* msg, int len, void* param);
	int  EncodeTXA(lpTXAInfo pTXAInfo, char byFrameBuff[1024]);
	int ParseEncodeSend(void);
	int encode4bdjobber(char* buffer2bdjobber, char * content, int bdid);
	int send2bdjobber(p2p_t p2p, char *buffer2bdjobber, int bytes);
#ifdef __cplusplus
}
#endif



#endif