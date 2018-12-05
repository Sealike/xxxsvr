#include "globaldata.h"
#include "json/cJSON.h"

epro_t epro_new()
{
	epro_t rtn = NULL;
	while((rtn = (epro_t)calloc(1,sizeof(epro_st))) == NULL) Sleep(10);
	return rtn;
}

void epro_free(epro_t data)
{
	free(data);
}

unsigned char checksum(unsigned char buffer[], int len)
{
	unsigned char data = 0;
	int i = 0;
	for (; i < len; i++)
		data ^= buffer[i];

	return data;
}

char* UTF8toGBK(char *pText)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, pText, -1, NULL, 0);
	unsigned short *wszgbk = malloc(sizeof(unsigned short)*(len + 1));
	memset(wszgbk, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, pText, -1, wszgbk, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszgbk, -1, NULL, 0, NULL, NULL);
	char *szgbk = malloc(len + 1);
	memset(szgbk, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszgbk, -1, szgbk, len, NULL, NULL);
	free(wszgbk);
	szgbk[len - 1] = '\0';
	return szgbk;
}