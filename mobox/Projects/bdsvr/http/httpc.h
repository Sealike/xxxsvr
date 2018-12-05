/********************************************************************  
filename:   HttpClient.h
created:    2016-04-08
author:     firehood  
purpose:    A Asynchronous Http Client By Using WinInet HTTP functions
*********************************************************************/
#pragma once

typedef struct _httpc_st
{
	void*  m_hInternet;
	void*  m_hSession;
	void*	   m_hRequestOpenedEvent;
	void*	   m_hRequestCompleteEvent;
	unsigned long	   m_dwCompleteResult;
}httpc_st, *httpc_t;

httpc_t httpc_new();
void    httpc_free(httpc_t);

int    httpc_open(httpc_t h,char* lpUrl, unsigned int timeout);
int    httpc_read(httpc_t h, unsigned char* buffer, int size, unsigned int timeout);
void   httpc_close(httpc_t h);
char*  httpc_doget(httpc_t h, char*   href, int opentimeouts, int readtimeouts);