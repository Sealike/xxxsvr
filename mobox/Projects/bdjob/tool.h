#ifndef __TOOL_H__
#define __TOOL_H__
#ifdef __cplusplus
extern "C"
{
#endif
int sendtoclient(int fd, char* strip,short port,char* str);

unsigned char checksum(unsigned char buffer[],int len);

int   translateMultiBtye(unsigned int  srcCodePage,const char* pSrcBuffer,unsigned long srclen,unsigned int  destCodePage,char *pDestBuffer,unsigned long destLen);


#define ISLCHAR(w)     (w>='a'&&w<='z')
#define ISHCHAR(w)     (w>='A'&&w<='Z')
#define ISDIGIT(w)     (w>='0'&&w<='9')
#define ISCHAR(w)       (ISDIGIT(w)|| ISLCHAR(w)||ISHCHAR(w))

#define NULLReturn(condition , rtnval)      {if((condition) == NULL)  return rtnval;}
#define FALSEReturn(condition, rtnval)      {if((condition) == FALSE) return rtnval;}
#define TRUEReturn(condition , rtnval)      {if((condition) == TRUE)  return rtnval;}
#ifdef __cplusplus
}
#endif
#endif