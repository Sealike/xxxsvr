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

	int id;				// 任务的序号
	char * desc;		// 任务描述
	char * maker;		// 任务制定者
	int taskid;			// 任务代号
	path_st path[10];	// 位置点集合
	int pathpointnum;	// path中位置点的个数
	char * userid;		// 用户代号
	char * maketime;	// 任务制定时间
	char * taskname;	// 任务名称


}bdjob_json_st, *bdjob_json_t;




typedef enum
{
	inet2bd = 0,
	sms2bd
}which2bd_t;


typedef struct tagTXAInfo
{
	int nLocalId;   //与外设相连的用户机ID号（新增）
	int  nDestId;   //此次通信的收信方地址。
	BYTE byCommCat; //通信类别，0-特快通信，1-普通通信。
	BYTE byCodeType;//传输方式，0-汉字，1-代码，2-混合。
	BOOL ifcmd;   //口令识别 0-通信，1-口令识别（新增）
	BOOL ifAnswer; //是否应答 当口令识别为0时，是否应答参数填全0  （新增）
	char chContent[1024];//通信内容.byCodeType ==0:每个汉字14bits,以计算机内码传输;byCodeType==1,每个bcd码占4bits，混发时，每个字节占8bits
	int  nContentLen;    //通信内容的字节数（电文长度）
	unsigned char byExpandType;		// 扩展类型，0-平台用户即时短信，1-手机号码即时短信，2-平台用户群发短信
	char FromPhoneNum[12];	// 通信发信方手机号码
	which2bd_t which2bd;
	//         byCodeType ==1:每个代码以一个ASCII标示。        
	//         byCodeType ==2:电文内容首字母固定为A4。     
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