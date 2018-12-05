// Rdss2_1SearchExp.h: interface for the CRdss2_1SearchExp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RDSS2_1SEARCHEXP_H__393A5412_ADD3_441C_9C8E_C93667C7548A__INCLUDED_)
#define AFX_RDSS2_1SEARCHEXP_H__393A5412_ADD3_441C_9C8E_C93667C7548A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./ChnavtectGlobal.h"
#include <vector>
#include <map>
#include "Struct.h"
using namespace std;

class CRdss2_1SearchExp:public CSearchExplainer
{
public:
	CRdss2_1SearchExp();
	virtual ~CRdss2_1SearchExp();	

	virtual int  ProcessData(const BYTE byBuffData[],const int nDataLen);
	virtual int  EncodeFrame(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen);
	virtual void Release(){delete this;}
	
protected:
	virtual BYTE DataExplain(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int  GetAccumulateCnt(UINT uMsgId);
	virtual BOOL CheckUserData(const void * pMsg,const UINT uMsgId);
	void CountRcvInfo(BYTE nInfoId);
	BYTE ComputeCheckSumRdss21(BYTE *Buff, int Len);

	virtual int GetTitleID(const char title[]);
	virtual int explain_klt(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_ect(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_gll(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_gbs(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_ztg(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_bsx(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_bsq(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_bsd(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_pky(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_icm(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_pmu(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_gsv(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_rmc(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_gga(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_bsi(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_dwr(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_fki(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_ici(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_icz(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_kls(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_hzr(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_txr(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_lzm(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_waa(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_sbx(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_zti(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_zda(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_gsa(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_gxm(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_dhv(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_xsd(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_xst(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_rns(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_apl(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_bid(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int explain_zhr(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);

    virtual int encode_rmo(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen);

	BYTE SearchData();
	void SetRdssTitles();
    
	BOOL IsValidTitles(char* strTitle);    	
	
	enum{ 
		READBUFFSIZE   = 500, 
		UNPACKBUFFSIZE = 1024};
	/* Êý¾ÝÖ¡ËÑË÷×´Ì¬ */
	enum {
		PACK_UNSEARCHED = 0,
		PACK_SEARCHED,
		PACK_STILLDATA,
		PACK_UNFINISHED,
		PACK_FINISHED,
	};
	/* Êý¾ÝËÑË÷×´Ì¬»ú */
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
protected:
	vector<string> m_vRdssTitle;
	typedef vector<string>::iterator itStr;
	tagICIInfo* m_ici;

private:
	BYTE  m_bSearchStatus;
	BYTE  m_ComBuff[UNPACKBUFFSIZE]; 
	DWORD m_dwComBufLen;
	DWORD m_dwCurrPt;
    BYTE  m_FrameBuff[UNPACKBUFFSIZE];
	DWORD m_dwFrameLen;
	BYTE  m_bCheckSum;
	DWORD m_wFrameIndLen;
    char  m_msgCache[2048];
	int m_nRef;
	map<UINT,UINT> m_mapProtCnt;
	BOOL           m_bBinaryMode;
	UINT           m_uBinaryLen;
	int            m_uLocalID;
};

#endif // !defined(AFX_RDSS2_1SEARCHEXP_H__393A5412_ADD3_441C_9C8E_C93667C7548A__INCLUDED_)
