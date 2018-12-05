/********************************************************************  
filename:   httpc.cpp
created:    2016-04-08
author:     firehood  
purpose:    A Asynchronous Http Client By Using WinInet HTTP functions
*********************************************************************/
#include <windows.h>
#include <wininet.h>

#include "httpc.h"
#include <tchar.h>
#include "../membuff.h"

#pragma comment(lib,"wininet.lib")

#define assure_buffer(buff){\
			while (buff == NULL) buff= membuff_new(64*1024);\
}

membuff_t g_mbuff = NULL;

void CALLBACK HttpStatusCallback(
	HINTERNET hInternet,
	DWORD dwContext,
	DWORD dwInternetStatus,
	LPVOID lpStatusInfo,
	DWORD dwStatusInfoLen);

httpc_t httpc_new(void)
{
	httpc_t rt = NULL;

	while ((rt = (httpc_t)calloc(1, sizeof(httpc_st))) == NULL) Sleep(1);
	
	rt->m_hInternet = NULL;
	rt->m_hSession = NULL;
	rt->m_dwCompleteResult = 0;
	rt->m_hRequestOpenedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	rt->m_hRequestCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	return rt;
}

void httpc_free(httpc_t http)
{
	if (http == NULL) return;

	httpc_close(http);

	if (http->m_hRequestOpenedEvent)
	{
		CloseHandle(http->m_hRequestOpenedEvent);
		http->m_hRequestOpenedEvent = NULL;
	}
	if (http->m_hRequestCompleteEvent)
	{
		CloseHandle(http->m_hRequestCompleteEvent);
		http->m_hRequestCompleteEvent = NULL;
	}
}

char*   httpc_doget(httpc_t http, char* href, int opentimeout, int readtimeout)
{
	char* rtn;	
	unsigned char  buffer[1025];
	int nRead = 0;

	assure_buffer(g_mbuff);

	BOOL bopen = httpc_open(http, href, opentimeout);
	if(!bopen) return "";

	while ((nRead = httpc_read(http,buffer, sizeof(buffer) - 1, readtimeout))>0){
		buffer[nRead] = '\0';
		membuff_addstr(g_mbuff, (char*)buffer, nRead);		
	}

	httpc_close(http);

	return g_mbuff->data;
}

int httpc_open(httpc_t http, char* szurl, int timeout)
{
	if (szurl == NULL)     return FALSE;

	if (http->m_hInternet) httpc_close(http);
	

	BOOL bRet = FALSE;
	do
	{
		http->m_hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, INTERNET_FLAG_ASYNC);

		if (http->m_hInternet == NULL) break;

		// Setup callback function
		if (InternetSetStatusCallback(http->m_hInternet, HttpStatusCallback) == INTERNET_INVALID_STATUS_CALLBACK)
		{
			break;
		}

		http->m_hSession = InternetOpenUrl(http->m_hInternet, szurl, NULL, NULL, INTERNET_FLAG_NO_CACHE_WRITE, (DWORD_PTR)http);
		if (NULL == http->m_hSession)
		{
			DWORD dw = GetLastError();
			if (GetLastError() != ERROR_IO_PENDING)
			{
				break;
			}
			// Wait until we get the connection handle
			if (WaitForSingleObject(http->m_hRequestOpenedEvent, timeout) == WAIT_TIMEOUT)
			{
				break;
			}
		}

		if (WaitForSingleObject(http->m_hRequestCompleteEvent, timeout) == WAIT_TIMEOUT)
		{
			break;
		}

		if (http->m_dwCompleteResult == 0)
		{
			break;
		}

		DWORD dwStatusCode;
		TCHAR responseText[256] = {0};
		DWORD responseTextSize = sizeof(responseText);
		if (!HttpQueryInfo(http->m_hSession, HTTP_QUERY_STATUS_CODE, &responseText, &responseTextSize, NULL))
		{
			break;
		}
		dwStatusCode = _ttoi(responseText);
		if(dwStatusCode != HTTP_STATUS_OK )
		{
			break;
		}
		bRet = TRUE;
	}while(0);

	if(!bRet)
	{
		if (http->m_hSession)
		{
			InternetCloseHandle(http->m_hSession);
			http->m_hSession = NULL;
		}
		if (http->m_hInternet)
		{
			InternetSetStatusCallback(http->m_hInternet, NULL);
			InternetCloseHandle(http->m_hInternet);
			http->m_hInternet = NULL;
		}
	}
	return bRet;
}

int httpc_read(httpc_t http, unsigned char* buffer, int size, int timeout)
{
	if(buffer == NULL || size <= 0)
	{
		return -1;
	}
	INTERNET_BUFFERS InetBuff;
	memset(&InetBuff, 0, sizeof(InetBuff));
	InetBuff.dwStructSize = sizeof(InetBuff);
	InetBuff.lpvBuffer = buffer;
	InetBuff.dwBufferLength = size;

	if (!InternetReadFileEx(http->m_hSession, &InetBuff, 0, (DWORD_PTR)http))
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			if (WaitForSingleObject(http->m_hRequestCompleteEvent, timeout) == WAIT_TIMEOUT)
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}
	return InetBuff.dwBufferLength;
}

void httpc_close(httpc_t http)
{
	SetEvent(http->m_hRequestOpenedEvent);
	SetEvent(http->m_hRequestCompleteEvent);
	if (http->m_hSession)
	{
		InternetCloseHandle(http->m_hSession);
		http->m_hSession = NULL;
	}
	if (http->m_hInternet)
	{
		InternetSetStatusCallback(http->m_hInternet, NULL);
		InternetCloseHandle(http->m_hInternet);
		http->m_hInternet = NULL;
	}
	ResetEvent(http->m_hRequestOpenedEvent);
	ResetEvent(http->m_hRequestCompleteEvent);
}

void CALLBACK HttpStatusCallback(
						HINTERNET hInternet,
						DWORD dwContext,
						DWORD dwInternetStatus,
						LPVOID lpStatusInfo,
						DWORD dwStatusInfoLen)
{
	httpc_t p = (httpc_t)dwContext;

	switch(dwInternetStatus)
	{
	case INTERNET_STATUS_HANDLE_CREATED:
		{
			INTERNET_ASYNC_RESULT *pRes = (INTERNET_ASYNC_RESULT *)lpStatusInfo;
			p->m_hSession = (HINTERNET)pRes->dwResult;
			SetEvent(p->m_hRequestOpenedEvent);
		}
		break;
	case INTERNET_STATUS_REQUEST_COMPLETE:
		{
			INTERNET_ASYNC_RESULT *pRes = (INTERNET_ASYNC_RESULT *)lpStatusInfo;
			p->m_dwCompleteResult = pRes->dwResult;
			SetEvent(p->m_hRequestCompleteEvent);
		}
		break;
	case INTERNET_STATUS_HANDLE_CLOSING:
		break;
	case INTERNET_STATUS_RESPONSE_RECEIVED:
		break;
	default:
		break;
	}
}
