#ifndef __TOOL_H__
#define __TOOL_H__

#define ISLCHAR(w)     (w>='a'&&w<='z')
#define ISHCHAR(w)     (w>='A'&&w<='Z')
#define ISDIGIT(w)     (w>='0'&&w<='9')
#define ISCHAR(w)       (ISDIGIT(w)|| ISLCHAR(w)||ISHCHAR(w))

#define NULLReturn(condition , rtnval)      {if((condition) == NULL)  return rtnval;}
#define FALSEReturn(condition, rtnval)      {if((condition) == FALSE) return rtnval;}
#define TRUEReturn(condition , rtnval)      {if((condition) == TRUE)  return rtnval;}

#define NULL     0
#define HEADLEN  12
#define FALSE    0
#define TRUE     1

#define _arraysize(_arr__) (sizeof(_arr__)/sizeof(_arr__[0]))

enum {
	PACK_UNSEARCHED = 0,
	PACK_SEARCHED,
	PACK_STILLDATA,
	PACK_UNFINISHED,
	PACK_FINISHED,
};

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



typedef struct _SMsgTitle_st{
	int id;
	char title[32];
}SMsgTitle_st, *SMsgTitle_t;


int sendtoclient(int fd, char* strip, short port, char* str);

unsigned char checksum(unsigned char buffer[], int len);

int   translateMultiBtye(unsigned int  srcCodePage, const char* pSrcBuffer, unsigned long srclen, unsigned int  destCodePage, char *pDestBuffer, unsigned long destLen);


#endif