#pragma once
#include <string>
#include "../Head.h"

using namespace std;

class CGlobalFunction
{
public:
	CGlobalFunction(void);
	~CGlobalFunction(void);
};

typedef UINT u32;

#define ISLCHAR(w)     (w>='a'&&w<='z')
#define ISHCHAR(w)     (w>='A'&&w<='Z')
#define ISDIGIT(w)     (w>='0'&&w<='9')

#define NULLReturn(condition , rtnval)      {if((condition) == NULL)  return rtnval;}
#define FALSEReturn(condition, rtnval)      {if((condition) == FALSE) return rtnval;}
#define TRUEReturn(condition , rtnval)      {if((condition) == TRUE)  return rtnval;}
#define BYTEBitSet(pbyte,maskflag)          {if((pbyte) != NULL) {(*pbyte) |= maskflag;}}
#define BYTEBitUnset(pbyte,maskflag)        {if((pbyte) != NULL) {(*pbyte) &= ~maskflag;}}

#define BYTEBitAsign(pbyte,maskflag,TorF)   {if((pbyte) != NULL){\
	if(TorF)\
		BYTEBitSet(pbyte,maskflag)\
	else\
		BYTEBitUnset(pbyte,maskflag);\
	}\
}

#define __array_size(array) (sizeof(array)/sizeof(array[0]))

#define __readint_4bytes(_buffer_,_iter_,data) \
{data = (unsigned char)(_buffer_)[_iter_++];data<<=8;data|=(unsigned char)(_buffer_)[_iter_++];data <<= 8;data|=(unsigned char)(_buffer_)[_iter_++];data<<=8;data|=(unsigned char)(_buffer_)[_iter_++];\
}

#define __readint_3bytes(_buffer_,_iter_,data) \
{data = (unsigned char)(_buffer_)[_iter_++];data<<=8;data|=(unsigned char)(_buffer_)[_iter_++];data <<= 8;data|=(unsigned char)(_buffer_)[_iter_++];\
}

#define __readint_2bytes(_buffer_,_iter_,_data_) \
{_data_ = (unsigned char)(_buffer_)[_iter_++];_data_<<=8;_data_|=(unsigned char)(_buffer_)[_iter_++];\
}

#define __read_byte(_buffer_,_iter_,data) \
{data = (unsigned char)(_buffer_)[_iter_++];\
}

#define __writeint_4bytes(_buffer_,_iter_,_nVal_)\
{(_buffer_)[_iter_++]=((_nVal_)>>24)&0xFF;\
	(_buffer_)[_iter_++]=((_nVal_)>>16)&0xFF;\
	(_buffer_)[_iter_++]=((_nVal_)>>8)&0xFF;\
	(_buffer_)[_iter_++]=(_nVal_)&0xFF;\
}

#define __writeint_3bytes(_buffer_,_iter_,_nVal_)\
{(_buffer_)[_iter_++]=((_nVal_)>>16)&0xFF;\
	(_buffer_)[_iter_++]=((_nVal_)>>8)&0xFF;\
	(_buffer_)[_iter_++]=(_nVal_)&0xFF;\
}

#define __writeint_2bytes(_buffer_,_iter_,_nVal_) \
{(_buffer_)[_iter_++]=((_nVal_)>>8)&0xFF;\
	(_buffer_)[_iter_++]=(_nVal_)&0xFF;\
}

#define __write_byte(_buffer_,_iter_,_nVal_) \
{(_buffer_)[_iter_++]=((_nVal_)&0xFF);\
}

#define __hex_char(_hexdata_,_chardata_) \
{   if((_hexdata_)<10&&(_hexdata_)>=0) \
		_chardata_ = (_hexdata_) + '0';\
	else if((_hexdata_)>=10&&(_hexdata_) <16)\
		_chardata_ = (_hexdata_) - 10 + 'A';\
	else _chardata_ ='0';\
}

#define __char_hex(_chardata_,_hexdata_) \
{\
	if((_chardata_)>='0' && (_chardata_)<='9')\
	_hexdata_ = _chardata_ - '0';\
	else if((_chardata_) >='A' && (_chardata_) <= 'F')\
	_hexdata_ = (_chardata_) -'A'+ 10;\
	else if((_chardata_) >='a' && (_chardata_) <= 'f')\
	_hexdata_ = (_chardata_) -'a'+ 10 ;\
	else _hexdata_ = 0;\
}

#define __DAY(data)     (((data)>>24)&0xFF)
#define __HOUR(data)    (((data)>>16)&0xFF)
#define __MINUTE(data)  (((data)>>8)&0xFF)
#define __SECOND(data)  (((data)>>0)&0xFF)

#define __UDEG(data)    ((int)(data))
#define __UMIN(data)    (((int)((data)*60.0))%60)
#define __USEC(data)    (((int)((data)*3600.0))%60)
#define __UDECSEC(data) (((int)((data)*36000.0))%10)

#define MAX2(data1,data2)        ((data1)>(data2) ? (data1) : (data2)) 
#define MAX3(data1,data2,data3)  (MAX2(MAX2(data1,data2),(data3)))

BOOL   IsHex(char chData);
int    IntTransfer(BOOL Direction, int Value, DWORD BitLen);
DWORD  RoundMax   (DWORD Dividend, DWORD Divisor);
void   ReverseByteSerial(BYTE *Buf, DWORD Len);

DWORD  GetUintFromBuff(void *SrcBuf, DWORD BeginBit, BYTE BitLen);
BOOL   WriteUintToBuff(void *Dest, DWORD BeginBit, DWORD BitLen, DWORD Data);

unsigned int  read_uint_from_bytes(unsigned char *destbuffer, unsigned int beginbit, unsigned char bitlen);
void   write_uint_to_bytes(unsigned char *destbuffer, unsigned int beginbit, unsigned int bitlen, unsigned int data);

int    AscToInt(char a, int &b);
char   IntToAsc(int a);

int    AscToHex(unsigned char *desthexbuff,unsigned int destbufflen,char *srcbuff,unsigned int srclen);
int    HexToAsc(char *destbuff,unsigned int destbufflen,unsigned char *srchexbuff,int srcbitscnt);

DWORD  CutTo(DWORD Value, DWORD Count);
/*chSysTypeBuff out*/
int    FrameSys(char * pFrame,char chSysTypeBuff[],int nBuffLen);
/*dfLat 单位:度*/
/*chBuffer out*/
int    FormatLatField(const double dfLat,char chBuffer[],const int nBufferLen);
/*dfLat 单位:度*/
/*chBuffer out*/
int    FormatLonField(const double dfLat,char chBuffer[],const int nBufferLen);
string LatField(const double dfLat);
string LonField(const double dfLon);

/* hexchars_bytes(..)
 * rtn :bits ;
 * srclen: bytes
 */
int  hexchars_bytes(unsigned char destbuffer[],int destbufflen,char srcbuffer[],int srclen);
int  bytes_hexchars(char DestBuffer[],int nDestBufferLen,unsigned char byByteArray[],int nBitsNum);

BOOL CheckDouble(const char* pstr);
BOOL CheckInt(const char* pstr);
BOOL CheckHMSTime(int nHour,int nMinute,float fSeconds);
int  FieldLength(const char* pstr);
int  IntpartLen(const char * pstr);
char* subfield(const char* str,char out[512]);
typedef struct _mytime_st 
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
}mytime_st,*mytime_t;

void mytime2str(mytime_t t,char* str);
void str2mytime(char* str,mytime_t t);
mytime_t mylocaltime(mytime_t dest);
unsigned char checksum(char* buff, int len);

//闭区间检查；
template<typename T,typename TS>
BOOL CheckCloseRange(T nData,TS nStart,TS nEnd)
{
	return (nData>=nStart && nData<=nEnd);
}

//开区间检查；
template<typename T,typename TS>
BOOL CheckOpenRange(T nData,TS nStart,TS nEnd)
{
	return (nData>nStart && nData<nEnd);
}

#define _IN_OPENDOMAIN(data,l,r)      (data>l&&data<r)
#define _IN_CLOSEDOMAIN(data,l,r)     (data>=l&&data<=r)
#define _IN_LEFTOPENDOMAIN(data,l,r)  (data>l&&data<=r)
#define _IN_RIGHTOPENDOMAIN(data,l,r) (data>=l&&data<r)

//半闭区间检查；
template<typename T,typename TS>
BOOL CheckLCloseRange(T nData,TS nStart,TS nEnd)
{
	return (nData>=nStart && nData<nEnd);
}

//半闭区间检查；
template<typename T,typename TS>
BOOL CheckRCloseRange(T nData,TS nStart,TS nEnd)
{
	return (nData>nStart && nData<=nEnd);
}

template<typename T,typename TS>
BOOL CheckInSet(T data,TS Set[],int nSetSize)
{
	BOOL bRtn = FALSE;

	if(Set != NULL && nSetSize>0)
	{
		int i = 0;
		for( ; i < nSetSize ; i++)
			if(data == Set[i])
			{
				bRtn = TRUE;
				break;
			}
	}

	return bRtn;
}

#define FRAME_SYS(frame,sysbuff,bufflen)  FrameSys(frame,sysbuff,bufflen);
#define CSTRLATFIELD(lat)  LatField(lat).c_str()
#define CSTRLONFIELD(lon)  LonField(lon).c_str()

/*
 *将“度”转换成 “度、分、秒、十分之一秒”格式。
 */
void   Deg2ArcDMS(double dfDegree,int &arcD,int &arcM,int &arcS,int &arcDecS);

/*
 *将度、分、秒、十分之一秒转换成“度”格式。
 */
double ArcDMS2Deg(int arcD,int arcM,int arcS,int arcDecS);