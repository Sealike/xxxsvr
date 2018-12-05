// IniWrap.cpp: implementation of the CIniWrap class.
//
//////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "IniWrap.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//#define USE_STATIC_PATH
CIniWrap::CIniWrap()
{
	memset(m_szIniFile, 0, sizeof(TCHAR) * MAX_PATH);
}

CIniWrap::CIniWrap(LPCTSTR pszFile)
{
	TCHAR pathname[MAX_PATH] = {'\0'};
	TCHAR dirve[MAX_PATH] = {'\0'};
	TCHAR dir[MAX_PATH] = {'\0'};
	TCHAR name[MAX_PATH] = {'\0'};
	TCHAR ext[MAX_PATH] = {'\0'};
	TCHAR* pch = NULL;
#ifdef USE_STATIC_PATH
	GetModuleFileName(NULL, pathname, MAX_PATH);
	_splitpath(pathname, dirve, dir, name, ext);
	sprintf(m_szIniFile, "%s%s.ini", pszFile, name);
#else
	GetModuleFileName(NULL, pathname, MAX_PATH);
	_tsplitpath(pathname, dirve, dir, name, ext);
	pch = _tcsrchr(dir, '\\');
	
	if(pch) {
		*pch = NULL; 
		pch = _tcsrchr(dir, '\\');
		if(pch) *pch = NULL;
	}
	_tcprintf(m_szIniFile, "%s%s\\etc\\%s.ini", dirve, dir, name);
#endif
	//strncpy(m_szIniFile, pszFile, MAX_PATH);
}

CIniWrap::~CIniWrap()
{
}

void CIniWrap::SetFile(LPCTSTR pszFile)
{
	_tcsncpy(m_szIniFile, pszFile, MAX_PATH);
}

BOOL CIniWrap::ReadStr(LPCTSTR pszSection,LPCTSTR pszKey,TCHAR* pszBuf,int iLen)
{
	return GetPrivateProfileString(pszSection,pszKey,_T(""),pszBuf,iLen,m_szIniFile);
}

BOOL CIniWrap::ReadInt(LPCTSTR pszSection,LPCTSTR pszKey,int& iRet,int iDefault)
{
	TCHAR szTemp[20];
	TCHAR szDefault[20];
	TCHAR * pszEnd = NULL;
	//itoa(iDefault, szDefault, 10);
	_tcprintf(szDefault,"%d",iDefault);
	BOOL fRet = GetPrivateProfileString(pszSection, pszKey, szDefault, szTemp, 100, m_szIniFile);
	iRet = _tcstol(szTemp,&pszEnd,10);
	return fRet;
}