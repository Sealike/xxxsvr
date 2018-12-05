#include "tool.h"
#include "winsock2.h"

int sendtoclient(int fd, char* strip,short port,char* str)
{
	int addrlen = sizeof(struct sockaddr_in);
	struct sockaddr_in sa = {0};

	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.S_un.S_addr = inet_addr(strip);

	return sendto(fd,str,strlen(str),0,(struct sockaddr*)&sa,addrlen);
}

unsigned char checksum(unsigned char buffer[],int len)
{
	unsigned char data = 0;
	int i = 0 ;
	for(; i < len ;i++)
		data ^= buffer[i];

	return data;
}

int   translateMultiBtye(unsigned int  srcCodePage,const char* pSrcBuffer,unsigned long srclen,unsigned int  destCodePage,char *pDestBuffer,unsigned long destLen)
{
	WCHAR wchar[2048];
	int size = MultiByteToWideChar(srcCodePage,0,pSrcBuffer,(int)(srclen+1),wchar,2048);
	if(size == 0)
	{
		strcpy(pDestBuffer,pSrcBuffer);
		size = (int)srclen;
	}
	else
	{
		size = WideCharToMultiByte(CP_ACP,0,wchar,size,pDestBuffer,(int)destLen,NULL,NULL);
		if(size == 0 )
		{
			strcpy(pDestBuffer,pSrcBuffer);
			size = (int)srclen;
		}
	}
	return size;
}