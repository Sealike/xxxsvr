// IniWrap.h: interface for the CIniWrap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIWRAP_H__34951268_FAEE_42EA_B340_0EC20841A5EA__INCLUDED_)
#define AFX_INIWRAP_H__34951268_FAEE_42EA_B340_0EC20841A5EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "type.h"
#ifndef _UNICODE
typedef const TCHAR*  LPCSTR;
#endif

#define MAX_PATH 256

class CIniWrap
{
public:
	CIniWrap();
	CIniWrap(LPCSTR pszFile);
	~CIniWrap();
public:
	//设置INI 文件名
	void SetFile(LPCSTR pszFile);
	BOOL ReadStr(LPCSTR pszSection,LPCSTR pszKey,char* pszBuf,int iLen=400);
	BOOL ReadInt(LPCSTR pszSection,LPCSTR pszKey,int& iRet,int iDefault=-1);
protected:
	char m_szIniFile[MAX_PATH];
};

#endif // !defined(AFX_INIWRAP_H__34951268_FAEE_42EA_B340_0EC20841A5EA__INCLUDED_)
