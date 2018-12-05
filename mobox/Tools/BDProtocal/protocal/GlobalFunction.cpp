#include "GlobalFunction.h"
#include <time.h>

CGlobalFunction::CGlobalFunction(void)
{
}

CGlobalFunction::~CGlobalFunction(void)
{
}

BOOL   IsHex(char chData)
{
	if(ISDIGIT(chData) || (chData>='A'&&chData<='F') || (chData>='a'&&chData<='f'))
		return TRUE;
	else
		return FALSE;
}

int  hexchars_bytes(unsigned char destbuffer[],int destbufflen, char srcbuffer[],int srclen){
	return AscToHex(destbuffer,destbufflen,srcbuffer, srclen);
}

int bytes_hexchars(char DestBuffer[],int nDestBufferLen,BYTE byBitArray[],int nBitsNum)
{
	return HexToAsc(DestBuffer,nDestBufferLen,byBitArray,nBitsNum);
}


/*
* �������ܣ��ŵ��з�������--������з����������໥ת��
* ��ڲ�����
* BOOL  Direction	������ƣ�1:�����-->�ŵ� 0:�ŵ�-->�����
* int   Value		��ת��ֵ
* DWORD BitLen		�ŵ�������ʾλ��
* ���ڲ�����
* int  ת�����
*/

/*
* �������ܣ��ŵ��з�������--������з����������໥ת��
* ��ڲ�����
* BOOL  Direction	������ƣ�1:�����-->�ŵ� 0:�ŵ�-->�����
* int   Value		��ת��ֵ
* DWORD BitLen		�ŵ�������ʾλ��
* ���ڲ�����
* int  ת�����
*/
int IntTransfer(BOOL Direction, int Value, DWORD BitLen)
{	
	DWORD Temp;
	if(BitLen>32)//BitLen>32ʱ,������������Ч��,�޷���λ
		return 0;
	if(Direction)	//1�������--�ŵ�
	{   
		if(Value<0)
		{
			Temp=1;	//����λ=1(��ֵ)
			Value=-Value;
		}
		else
			Temp=0;	//����λ=0(��ֵ)
		Value = CutTo(Value, BitLen-1);
		return (Temp<<(BitLen-1))|Value;
	}
	else	//0���ŵ�--�����
	{
		Temp = 1;
		Temp = Value&(Temp<<(BitLen-1));	//����λ����λ��
		Value = CutTo(Value, BitLen-1);
		if(Temp)	//����λ=1(��ֵ)
			return Value*(-1);
		else
			return Value;
	}
}

/*
* �������ܣ�������������ȡ��
* ��ڲ�����
* DWORD Dividend	������
* DWORD Divisor	����
*/
DWORD RoundMax(DWORD Dividend, DWORD Divisor)
{
	if(Dividend % Divisor != 0)
		return Dividend/Divisor + 1;
	else
		return Dividend/Divisor;
}

/*
* �������ܣ��ߵ��ֽڸߵ�˳��
* ��ڲ�����
* BYTE  Buf	��ת�����ֽ�����
* DWORD Len	�ֽ���
*/
void ReverseByteSerial(BYTE *Buf, DWORD Len)
{   
	DWORD f_dwWordLen = RoundMax(Len, 4);
	DWORD f_dwIndex;
	BYTE  f_bTemp;

	for(DWORD i=0; i<f_dwWordLen; i++)
	{
		f_dwIndex = 4*i;
		f_bTemp = Buf[f_dwIndex+3];
		Buf[f_dwIndex+3] = Buf[f_dwIndex];
		Buf[f_dwIndex] = f_bTemp;
		f_bTemp = Buf[f_dwIndex+2];
		Buf[f_dwIndex+2] = Buf[f_dwIndex+1];
		Buf[f_dwIndex+1] = f_bTemp;
	}
}

DWORD GetUintFromBuff(void *SrcBuf, DWORD BeginBit, BYTE BitLen)
{
	if(BitLen==0)
		return 0;
	if(BitLen>32)
		BitLen = 32;

	DWORD f_dwBeginWord, f_dwTemp;

	f_dwBeginWord = BeginBit / 32;
	BeginBit = BeginBit % 32;
	f_dwTemp = ((DWORD*)SrcBuf)[f_dwBeginWord] << BeginBit;//�����λ���룻
	if(BitLen <= (32-BeginBit))//
		return (f_dwTemp >> (32-BitLen));//ok
	else
	{
		f_dwTemp |= (((DWORD*)SrcBuf)[f_dwBeginWord+1] >> (32-BeginBit));
		return (f_dwTemp >> (32-BitLen));
	}
}

BOOL WriteUintToBuff(void *Dest, DWORD BeginBit, DWORD BitLen, DWORD Data)
{
	//�Ϸ����ж�
	if(BitLen > 32 || BitLen <= 0)
		return false ;
	if(BeginBit < 0)
		return false ;
	
	Data = Data << ( 32-BitLen ) ;  // ������ʹ��Чλ�����ֵȫΪ��
	Data = Data >> ( 32-BitLen ) ;

	DWORD* DestBuff = (DWORD*)Dest ;
	DWORD dwTmp_BeginBit,dwindex, dwTem1 ,dwTem2 ;
    
	dwTmp_BeginBit = BeginBit % 32 ;
	dwindex = BeginBit / 32 ;
    
	if(BitLen <= (32 - dwTmp_BeginBit))
	{   //���������е�����
		DestBuff[dwindex] = DestBuff[dwindex] >> (32 - dwTmp_BeginBit ) ;
		DestBuff[dwindex] = DestBuff[dwindex] << (32 - dwTmp_BeginBit )	;
		//�ƶ���Чλ��dwTmp_BeginBit ָ����λ��
		dwTem2 = Data << ( 32 - dwTmp_BeginBit - BitLen) ;
		//��ֵ
		DestBuff[dwindex] = DestBuff[dwindex] + dwTem2 ;
	}
	else
	{    
		//�����һ˫�ӵ�����
		//������һ��˫�ֽ�
		DestBuff[dwindex] = DestBuff[dwindex] >> (32 - dwTmp_BeginBit ) ;
		DestBuff[dwindex] = DestBuff[dwindex] << (32 - dwTmp_BeginBit ) ;
		//ȡ���ڵ�һ���ֽ��е���Чλ���Ҹ�ֵ������
		DestBuff[dwindex] += ( Data >> ( BitLen - ( 32 - dwTmp_BeginBit ) ) );		
		//����λ�ڵڶ��ӽ������
		//�����ڶ��ӽ�������
		DestBuff[dwindex+1] = 0;
		//ȡ�õڶ��ֽ��е���Чλ����ֵ���ڶ����ֽ�
		dwTem1 = Data << ( 32 - ( BitLen - ( 32 - dwTmp_BeginBit ) ) );
		DestBuff[dwindex+1] = dwTem1 ;			
	}

	return TRUE;
}

int AscToInt(char a, int &b)
{
	if(a>='0'&&a<='9')
	{
		b=a-48;
		return 0;
	}else if(a>='a'&&a<='f')
	{	
		b=a-87;
		return 0;
	}else if(a>='A'&&a<='F')
	{
		b=a-55;
		return 0;
	}else
		return -1;
}

char IntToAsc(int a)
{
	a = a&0xf;
	if(a<10)
		return a+'0';
	else
		return a-10+'A';
}


/*
 *rtn :bits;
 */
int AscToHex(unsigned char *desthexbuff,unsigned int destbufflen,char *srcbuff,unsigned int srclen)
{
    unsigned char data;
	int rtn = 0;

	for(size_t i = 0 ; i < srclen ; i++){
		__char_hex(srcbuff[i],data);
		if(i%2 == 0){
			desthexbuff[i/2] = data<<4;
			rtn += 4;
		}else{
			desthexbuff[i/2] |= data;
			rtn += 4;
		}
		
		if(rtn == destbufflen*8){
			rtn = destbufflen*8;
			break;
		}
	}

	return rtn;
}

int  HexToAsc(char *destbuff,unsigned int destbufflen,unsigned char *srchexbuff,int srcbitscnt)
{
	char chdata;
	int i = 0 ;

	for(; i < srcbitscnt;)
	{
		if(i%8==0){
			__hex_char(srchexbuff[i/8]>>4,chdata);
		}else{
			__hex_char(srchexbuff[i/8]&0x0F,chdata);
		}

		destbuff[i/4] = chdata;
		i+=4;		
	}

	return i/4;
}

//��������Ч��ֵ
DWORD CutTo(DWORD Value, DWORD Count)
{
	DWORD OffSet=32-Count%33;
	if(Count==32)	//Count=BitLen-1,��ʱ32λ������Чλ
		return Value;
	if(Count==0)	//û����Чλ
		return 0;
	else
		return (Value<<OffSet)>>OffSet;	//ʹ����λ���
}

//����ֵ:ϵͳ�����ַ�������,����'\0'
int FrameSys(char * pFrame,char chSysType[],int nBuffLen)
{  
	NULLReturn(pFrame,0);
    NULLReturn(chSysType,0);

    FALSEReturn(nBuffLen>2 , 0);
    
    char * pstr = pFrame;
	while(*pstr == '$')
	{
		pstr ++;
	}
    
    chSysType[0] = *(pstr++);
	chSysType[1] = *(pstr++);
	chSysType[2] = '\0';
    
	return 3;
}

int  FormatLatField(const double dfLat,char chBuffer[],const int nBufferLen)
{   
	NULLReturn (chBuffer,FALSE);
	FALSEReturn(nBufferLen > 7,FALSE);
    
	return sprintf_s(chBuffer,nBufferLen,"%02d%05.2f",(int)dfLat,(dfLat - (int)dfLat)*60);
	
}

int  FormatLonField(const double dfLon,char chBuffer[],const int nBufferLen)
{   
	NULLReturn (chBuffer,FALSE);
	FALSEReturn(nBufferLen > 8,FALSE);
    
	return sprintf_s(chBuffer,nBufferLen,"%03d%05.2f",(int)dfLon,(dfLon - (int)dfLon)*60);
}

string LatField(const double dfLat)
{
	char chBuffer[10];
	sprintf_s(chBuffer,10,"%03d%05.2f",(int)dfLat,(dfLat - (int)dfLat)*60);
	return string(chBuffer);
}

string LonField(const double dfLon)
{
	char chBuffer[10];
	sprintf_s(chBuffer,10,"%03d%05.2f",(int)dfLon,(dfLon - (int)dfLon)*60);
	return string(chBuffer);
}

BOOL CheckDouble(const char* pstr)
{
	BOOL   bRtn = FALSE;
	char * pEnd = NULL;
	
	double dfData = strtod( pstr, &pEnd);
    if(*pEnd==',' || *pEnd=='\0'||*pEnd=='*')
		bRtn = TRUE;
	
	return bRtn;
}

BOOL CheckInt(const char* pstr)
{	
	BOOL   bRtn = FALSE;
	char * pEnd = NULL ;
	
	int nData = strtol( pstr, &pEnd,10);
    if(*pEnd==',' || *pEnd=='\0'||*pEnd=='*')
		bRtn = TRUE;
	
	return bRtn;
}

int  FieldLength(const char* str)
{
	int i = 0;
	for(; (*str) != NULL && (*str != ',')&&(*str) != '*';i++)
	{
		str++;
		if(i==1024)//����û�д���1024���ֶΡ�
			break;
	}

	return i;
}

unsigned int  read_uint_from_bytes(unsigned char *destbuffer, unsigned int beginbit, unsigned char bitlen)
{
	DWORD dwRslt = 0;

	if(bitlen > 32) return -1;

	for(int i= 0 ; i< bitlen ; i ++){
		if(destbuffer[(beginbit+i)/8]>>(7-((beginbit+i)%8))&0x01)
			dwRslt |= 1<<(bitlen-i-1);		
	}

	return dwRslt;
}

void   write_uint_to_bytes(unsigned char *destbuffer, unsigned int beginbit, unsigned int bitlen, unsigned int data)
{
	if(destbuffer == NULL || bitlen == 0)
		return ;
	
	for(size_t i = 0 ; i< bitlen ; i++)
	{
		if((beginbit + i)%8 == 0)
			destbuffer[(beginbit + i)/8] = 0;			
		
		if((data >> (bitlen - i -1))&0x01)
	        destbuffer[(beginbit + i)/8] |= (1<< (7-((beginbit + i)%8)));
	}
}

BOOL CheckHMSTime(int nHour,int nMinute,float fSeconds)
{
	BOOL bCheck = (CheckCloseRange(nHour,0,23) && CheckCloseRange(nMinute,0,59) && CheckLCloseRange(nMinute,0,60));
	return bCheck;
}

int  IntpartLen(const char * pstr)
{
	int i = 0 ;
	for(; i < 32 ;i++)
	{
		if(ISDIGIT(pstr[i]))
			continue;
		else
			break;
	}

	return i;
}

char* subfield(const char* str,char out[512])
{
	int count =0 ;
	const char* pos_end;
	const char* pos_start = strchr(str,',');
	if(pos_start == NULL) return NULL;

	pos_end  = strchr(pos_start+1,',');

	if(pos_start&&pos_end)
	{
		int i = 0 ;
		for(; i < pos_end - pos_start-1 ; i++)
			out[count++]=(*(pos_start+1+i));//

	}else if(pos_end == NULL){
		pos_start++;
		while((*pos_start)!='\0')
			out[count++]=(*pos_start++);
	}
	out[count] = '\0';

	return (char*)pos_end;
}

void mytime2str(mytime_t t,char* str)
{
	if(t == NULL || str == NULL) return;
	sprintf(str,"%d-%02d-%02d %02d:%02d:%02d",t->year,t->month,t->day,t->hour,t->minute,t->second);
}

void str2mytime(char* str,mytime_t t)
{
	char* p = str;

	if(t == NULL || str == NULL) return;
	//"Y-M-D h:m:s"

	while(*p!='\0')
	{
		while(*p == ' ')p++;
		t->year = atoi(p);

		while(*p != '-')p++;
		p++;
		while(*p == ' ')p++;
		t->month = atoi(p);

		while(*p != '-')p++;
		p++;
		while(*p == ' ')p++;
		t->day = atoi(p);

		while(*p != ' ')p++;
		while(*p == ' ')p++;
		t->hour = atoi(p);

		while(*p != ':')p++;
		p++;
		while(*p == ' ')p++;
		t->minute = atoi(p);

		while(*p != ':')p++;
		p++;
		while(*p == ' ')p++;
		t->second = atoi(p);
		break;
	}
}

unsigned char checksum(char* buff, int len)
{
	unsigned char rtn = 0;
	int i = 0;
	for(; i<len; i++)
		rtn ^= buff[i];

	return rtn;
}

mytime_t mylocaltime(mytime_t dest)
{
	struct tm t;
	if(dest == NULL) return NULL;

	_getsystime(&t);
	t.tm_year += 1900;
	t.tm_mon  += 1;

	dest->year = t.tm_year;
	dest->month = t.tm_mon;
	dest->day = t.tm_mday;
	dest->hour = t.tm_hour;
	dest->minute = t.tm_min;
	dest->second = t.tm_sec;

	return dest;
}

char* b_strstr(char * pstr,int pstrlen,char* strtag,int strtaglen)
{
	int index = 0;
	int pos = 0;

	for(int i = 0 ; i < pstrlen ;){

		if(pstr[i] == strtag[index]){
			if(index == 0) pos = i;
			index++;
			if(index == strtaglen)
				break;
		}else if(index > 0 && pstr[i] != strtag[index]){
			index = 0;
			i=pos;			
		}
		i++;
	}

	if(index == strtaglen)
		return &pstr[pos];
	else 
		return NULL;
}

void Deg2ArcDMS(double dfDegree,int &arcD,int &arcM,int &arcS,int &arcDecS)
{
	arcD = (int)dfDegree;

	arcM = (int)((dfDegree -arcD)*60.0);

	arcS = (int)((dfDegree -arcD - arcM/60.0)*3600.0);

	arcDecS = (int)((dfDegree -arcD - arcM/60.0 - arcS/3600.0)*36000.0);	
}

double ArcDMS2Deg(int arcD,int arcM,int arcS,int arcDecS)
{
	return (arcD + arcM/60.0 + arcS/3600.0+arcDecS/36000.0);
}