#ifndef __RDSS_40_SEARCHEXP__H
#define __RDSS_40_SEARCHEXP__H

#include <vector>
#include <map>

#include "./ChnavtectGlobal.h"
//#include "ChnavtectGlobal.h"
#include "Struct.h"
#include "../Head.h"

using namespace std;

struct extendedplugin_st{
	char * title;
	int  id;
	pCBExt_explain pExpalin;
	void* param;
	extendedplugin_st(){title = NULL;id = NULL;pExpalin = NULL;param = NULL;}
};

struct tagICIInfo;
class CRdss4_0SearchExp  : public CSearchExplainer
{
public:
	CRdss4_0SearchExp();
	virtual ~CRdss4_0SearchExp();
	virtual void Release(){delete this;}
	int  ProcessData(const BYTE byBuffData[] ,const int nDataLen);//数据处理
	int  EncodeFrame(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen);
	BYTE DataExplain(BYTE byFrame[] , int nFrameLen,BYTE *DataBuff, DWORD &Len);//数据解析
	int  GetAccumulateCnt(UINT uMsgId);
	virtual BOOL AddExtenedPlugin(int msgid,char* title,pCBExt_explain exp_cb);
protected:
	BYTE excuteplugin(BYTE byFrame[] , int nFrameLen,BYTE *DataBuff, DWORD &Len);//数据解析
	int  CheckTitleType(char* title);
	BYTE SearchData();
	BYTE Checksum(BYTE byBuffer[],int len);

	virtual int explain_hzr(BYTE byFrame[] , int nFrameLen,BYTE *DataBuff, DWORD &Len);
	virtual int explain_zhr(BYTE byFrame[] , int nFrameLen,BYTE *DataBuff, DWORD &Len);
	virtual int explain_sbx(BYTE byFrame[] , int nFrameLen,BYTE *DataBuff, DWORD &Len);
	virtual int explain_gxm(BYTE byFrame[] , int nFrameLen,BYTE *DataBuff, DWORD &Len);
	virtual int explain_lzm(BYTE byFrame[] , int nFrameLen,BYTE *DataBuff, DWORD &Len);
	virtual int explain_zda(BYTE byFrame[] , int nFrameLen,BYTE *DataBuff, DWORD &Len);
	virtual int explain_gga(BYTE byFrame[] , int nFrameLen,BYTE *DataBuff, DWORD &Len);
	virtual int explain_ici(BYTE byFrame[] , int nFrameLen,BYTE *DataBuff, DWORD &Len);
	virtual int explain_txr(BYTE byFrame[] , int nFrameLen,BYTE *DataBuff, DWORD &Len);
	virtual int explain_dwr(BYTE byFrame[] , int nFrameLen,BYTE *DataBuff, DWORD &Len);
	virtual int explain_fki(BYTE byFrame[] , int nFrameLen,BYTE *DataBuff, DWORD &Len);
	virtual int explain_bsi(BYTE byFrame[] , int nFrameLen,BYTE *DataBuff, DWORD &Len);
	virtual int explain_zti(BYTE byFrame[] , int nFrameLen,BYTE *DataBuff, DWORD &Len);

	virtual int encode_cxa(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen);
	virtual int encode_zhs(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen);
	virtual int encode_jms(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen);
	virtual int encode_gxm(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen);
	virtual int encode_rmo(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen);
	virtual int encode_ica(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen);
	virtual int encode_txa(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen);
	virtual int encode_waa(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen);
	virtual int encode_dwa(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen);
	virtual int encode_gljc(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen);
	virtual int encode_xtzj(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen);
	virtual int encode_lzm(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen);

	void SetRdssTitles();	
	BOOL IsValidTitles(char* strTitle);    
	int  EncodeFrame_rmo(lpRMOInfo pRmoInfo ,unsigned char frame[],int& len);
	int  EncodeStopInstruct(char* title,int type,unsigned char frame[],int &iter);
	int  adjustcontent(char content[],int contentlen/*bytes*/,int codetype,BOOL urgent,outbuffer_t data);
	int  m_uLocalID;
	enum{ 
		READBUFFSIZE   = 500, 
		UNPACKBUFFSIZE = 1024
	};
	/* 数据帧搜索状态 */
	enum {
		PACK_UNSEARCHED = 0,
		PACK_SEARCHED,
		PACK_STILLDATA,
		PACK_UNFINISHED,
		PACK_FINISHED,
	};
	/* 数据搜索状态机 */
	enum {
		SEARCHMACHINE_NULL = 0,
		SEARCH_GETHEAD,
		SEARCH_GETDATA,
		SEARCH_GETCHECK,
		SEARCHMACHINE_BEGININD,
		SEARCHMACHINE_IDENTIFIER,
		SEARCHMACHINE_LENGTH,
		SEARCHMACHINE_BINARYCHECK,
	};

	typedef vector<string>::   iterator           itStr;
	typedef vector<CSinker *>::iterator           SinkerIt;

	BYTE  m_bSearchStatus;
	BYTE  m_ComBuff[UNPACKBUFFSIZE]; 
	DWORD m_dwComBufLen;
	DWORD m_dwCurrPt;
	BYTE  m_FrameBuff[UNPACKBUFFSIZE];
	DWORD m_dwFrameLen;
	BYTE  m_bCheckSum;
	DWORD m_wFrameIndLen;

	BYTE  m_msgCache[2048];
	vector<string> m_vRdssTitle;
	BOOL  m_bBinaryMode;
	UINT  m_uBinaryLen;
	tagICIInfo* m_ici;
	vector<extendedplugin_st> m_plugins;
	CLockCriticalSection g_lock_plugin;
};

#endif