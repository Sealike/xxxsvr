// Rdss2_1SearchExp.h: interface for the CRdss2_1SearchExp_shangyu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RDSS2_1SEARCHEXP_SHANGYU_H__393A5412_ADD3_441C_9C8E_C93667C7548A__INCLUDED_)
#define AFX_RDSS2_1SEARCHEXP_SHANGYU_H__393A5412_ADD3_441C_9C8E_C93667C7548A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Rdss2_1SearchExp.h"
#include "protocal/ChnavtectGlobal.h"
#include <vector>
#include <map>
#include "Struct.h"
using namespace std;

class CRdss2_1SearchExp_shangyu:public CRdss2_1SearchExp
{
public:
	CRdss2_1SearchExp_shangyu();
	virtual ~CRdss2_1SearchExp_shangyu();	

	//virtual int  EncodeFrame(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen);
	virtual void Release(){delete this;}
	
protected:
	virtual int GetTitleID(const char title[]);
	virtual BOOL CheckUserData(const void * pMsg,const UINT uMsgId);
	virtual int explain_zti(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len);
	virtual int encode_rmo(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen);
	void SetRdssTitles();
	//BOOL IsValidTitles(char* strTitle); 

};

#endif // !defined(AFX_RDSS2_1SEARCHEXP_H__393A5412_ADD3_441C_9C8E_C93667C7548A__INCLUDED_)
