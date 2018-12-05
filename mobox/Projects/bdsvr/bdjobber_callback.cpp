#include "bdjobber_callback.h"
#include "frontsvr_callback.h"
#include "CirQueue.h"
#include "p2p.h"
#include "protobuffer.h"
#include "bdjobber_expat.h"
#include "json/cJSON.h"
#include "tool.h"
#include "include/ConstDefine.h"
#include "jsonstring.h"
#include <stdio.h>
#include "dbaccess.h"
#include "mysql.h"
#include "myquery/mysqldb.h"
#include "jobqueue.h"

#include <string>
using namespace std;

#include "instapp.h"
#include "malloc.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern BOOL jobber_free;
extern instapp_t g_instapp;


int Seqno2FrontsvrManage(void);


#ifdef __cplusplus
}
#endif


#define SEQNO2FRONTSVR  102
#define BD2FRONTSVR_SEQNO_MAX 65536		// 远小于unsigned int 型数据的最大值

static unsigned char checksum(unsigned char buffer[], int len)
{
	unsigned char data = 0;
	int i = 0;
	for (; i < len; i++)
		data ^= buffer[i];

	return data;
}


static unsigned int  read_uint_from_bytes(unsigned char *destbuffer, unsigned int beginbit, unsigned char bitlen)
{
	DWORD dwRslt = 0;

	if (bitlen > 32) return -1;

	for (int i = 0; i < bitlen; i++){
		if (destbuffer[(beginbit + i) / 8] >> (7 - ((beginbit + i) % 8)) & 0x01)
			dwRslt |= 1 << (bitlen - i - 1);
	}

	return dwRslt;
}

//闭区间检查；
template<typename T, typename TS>
BOOL CheckCloseRange(T nData, TS nStart, TS nEnd)
{
	return (nData >= nStart && nData <= nEnd);
}

//半闭区间检查；
template<typename T, typename TS>
BOOL CheckLCloseRange(T nData, TS nStart, TS nEnd)
{
	return (nData >= nStart && nData < nEnd);
}
int  IntpartLen(const char * pstr)
{
	int i = 0;
	for (; i < 32; i++)
	{
		if (ISDIGIT(pstr[i]))
			continue;
		else
			break;
	}

	return i;
}

BOOL CheckInt(const char* pstr)
{
	BOOL   bRtn = FALSE;
	char * pEnd = NULL;

	int nData = strtol(pstr, &pEnd, 10);
	if (*pEnd == ',' || *pEnd == '\0' || *pEnd == '*')
		bRtn = TRUE;

	return bRtn;
}

int  FieldLength(const char* str)
{
	int i = 0;
	for (; (*str) != NULL && (*str != ',') && (*str) != '*'; i++)
	{
		str++;
		if (i == 1024)//长度没有大于1024的字段。
			break;
	}

	return i;
}

BOOL   IsHex(char chData)
{
	if (ISDIGIT(chData) || (chData >= 'A'&&chData <= 'F') || (chData >= 'a'&&chData <= 'f'))
		return TRUE;
	else
		return FALSE;
}

template<typename T, typename TS>
BOOL CheckInSet(T data, TS Set[], int nSetSize)
{
	BOOL bRtn = FALSE;

	if (Set != NULL && nSetSize > 0)
	{
		int i = 0;
		for (; i < nSetSize; i++)
			if (data == Set[i])
			{
				bRtn = TRUE;
				break;
			}
	}

	return bRtn;
}

BOOL CheckDouble(const char* pstr)
{
	BOOL   bRtn = FALSE;
	char * pEnd = NULL;

	double dfData = strtod(pstr, &pEnd);
	if (*pEnd == ',' || *pEnd == '\0' || *pEnd == '*')
		bRtn = TRUE;

	return bRtn;
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


static int AscToHex(unsigned char *desthexbuff, unsigned int destbufflen, char *srcbuff, unsigned int srclen)
{
	unsigned char data;
	int rtn = 0;

	for (size_t i = 0; i < srclen; i++)
	{
		__char_hex(srcbuff[i], data);
		if (i % 2 == 0)
		{
			desthexbuff[i / 2] = data << 4;
			rtn += 4;
		}
		else
		{
			desthexbuff[i / 2] |= data;
			rtn += 4;
		}

		if (rtn == destbufflen * 8)
		{
			rtn = destbufflen * 8;
			break;
		}
	}

	return rtn;
}

static int  hexchars_bytes(unsigned char destbuffer[], int destbufflen, char srcbuffer[], int srclen){
	return AscToHex(destbuffer, destbufflen, srcbuffer, srclen);
}



static int  HexToAsc(char *destbuff, unsigned int destbufflen, unsigned char *srchexbuff, int srcbitscnt)
{
	char chdata;
	int i = 0;

	for (; i < srcbitscnt;)
	{
		if (i % 8 == 0){
			__hex_char(srchexbuff[i / 8] >> 4, chdata);
		}
		else{
			__hex_char(srchexbuff[i / 8] & 0x0F, chdata);
		}

		destbuff[i / 4] = chdata;
		i += 4;
	}

	return i / 4;
}

int Seqno2FrontsvrManage(void)
{
	if (++g_instapp->seqno2frontsvr >= BD2FRONTSVR_SEQNO_MAX)
		g_instapp->seqno2frontsvr = SEQNO2FRONTSVR;

	/*char* config_file = "../etc/bdsvr.xml";
	char ctemp[6] = { 0 };
	_itoa(g_instapp->seqno2frontsvr, ctemp, 10);
	char * rt = config_set_one(g_instapp->config, "seqno.tofrontsvr", ctemp, 1, config_file, 1);
*/
	return 0;
}

void warn2frontsvr(evtreport_t evt, bd_evt_type_t type)
{
	// step1. json串加前后内容，组成位置报告请求报文
	char buffer[2048];
	int count = 0;
	int endpos = 0;
	response_pkt_p pkt;
	char * warntype = NULL;

	if (type == warn_low_power)
		warntype = "lowpower";
	if (type == warn_destroy)
		warntype = "destroy";

	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$warn,%s,%s{\"taskid\":\"%s\",\"executor\":\"%s\",\"net\":\"%s\",\"data\":{\"time\":\"%s\"\"type\":\"%s\"}}", g_instapp->auth_name, "frontsvr", evt->taskid, evt->executor,"bd", evt->time, warntype);
	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
	endpos = count;
	buffer[endpos + 1] = '\0';

	// step2. 发送
	pkt = response_pkt_new(((p2pclient_t)(g_instapp->p2p_frontsvr))->pktpool, endpos + 1);
	memcpy(pkt->data, buffer, endpos + 1);
	pkt->len = endpos + 1;
	p2pclient_write((p2pclient_t)(g_instapp->p2p_frontsvr), pkt);

	Seqno2FrontsvrManage();
}

void pos2frontsvr(bdposreport_t pos)
{
	// step1. json串加前后内容，组成位置报告请求报文
	char buffer[2048];
	int count = 0;
	int endpos = 0;
	response_pkt_p pkt;

	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$pos,%s,%s{\"taskid\":\"%s\",\"executor\":\"%s\",\"net\":\"%s\",\"data\":{\"time\":\"%s\",\"lon\":\"%.8f\",\"lat\":\"%.8f\",\"hgt\":\"%.8f\"}}", g_instapp->auth_name, "frontsvr", pos->taskid,pos->executor,"bd",pos->time,pos->longtitue,pos->lattitude,pos->height);
	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
	endpos = count;
	buffer[endpos + 1] = '\0';

	// step2. 发送
	pkt = response_pkt_new(((p2pclient_t)(g_instapp->p2p_frontsvr))->pktpool, endpos + 1);
	memcpy(pkt->data, buffer, endpos + 1);
	pkt->len = endpos + 1;
	p2pclient_write((p2pclient_t)(g_instapp->p2p_frontsvr), pkt);

	Seqno2FrontsvrManage();
}

void reqcode2frontsvr(evtreport_t evt)
{
	// step1. json串加前后内容，组成位置报告请求报文
	char buffer[2048];
	int count = 0;
	int endpos = 0;
	response_pkt_p pkt;

	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$reqcode,%s,%s{\"taskid\":\"%s\",\"executor\":\"%s\",\"seqno\":\"%d\",\"net\":\"%s\",\"data\":{\"time\":\"%s\",\"hid\":\"%s\",\"digest\":\"%s\"}}", g_instapp->auth_name, "frontsvr", evt->taskid, evt->executor, g_instapp->seqno2frontsvr, "bd",evt->time, evt->hid, evt->digest);
	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
	endpos = count;
	buffer[endpos + 1] = '\0';

	// step2. 发送
	pkt = response_pkt_new(((p2pclient_t)(g_instapp->p2p_frontsvr))->pktpool, endpos + 1);
	memcpy(pkt->data, buffer, endpos + 1);
	pkt->len = endpos + 1;
	p2pclient_write((p2pclient_t)(g_instapp->p2p_frontsvr), pkt);

	printf("reqcode2frontsvr: %s\n",buffer);

	Seqno2FrontsvrManage();
}

void taskpau2frontsvr(evtreport_t evt)
{
	// step1. json串加前后内容，组成位置报告请求报文
	char buffer[2048];
	int count = 0;
	int endpos = 0;
	response_pkt_p pkt;

	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$taskpau,%s,%s{\"taskid\":\"%s\",\"executor\":\"%s\",\"seqno\":\"%d\",\"net\":\"%s\",\"data\":[{\"time\":\"%s\",\"desc\":\"%s\"}]}", g_instapp->auth_name, "frontsvr", evt->taskid, evt->executor, "bd",g_instapp->seqno2frontsvr, evt->time, evt->content);
	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
	endpos = count;
	buffer[endpos + 1] = '\0';

	// step2. 发送
	pkt = response_pkt_new(((p2pclient_t)(g_instapp->p2p_frontsvr))->pktpool, endpos + 1);
	memcpy(pkt->data, buffer, endpos + 1);
	pkt->len = endpos + 1;
	p2pclient_write((p2pclient_t)(g_instapp->p2p_frontsvr), pkt);

	Seqno2FrontsvrManage();
}

void taskcon2frontsvr(evtreport_t evt)
{
	// step1. json串加前后内容，组成位置报告请求报文
	char buffer[2048];
	int count = 0;
	int endpos = 0;
	response_pkt_p pkt;

	count += sprintf_s(buffer + count, sizeof(buffer) - count, "$taskcon,%s,%s{\"taskid\":\"%s\",\"executor\":\"%s\",\"seqno\":\"%d\",\"net\":\"%s\",\"data\":[{\"time\":\"%s\",\"desc\":\"%s\"}]}", g_instapp->auth_name, "frontsvr", evt->taskid, evt->executor, g_instapp->seqno2frontsvr,"bd", evt->time, evt->content);
	count += sprintf_s(buffer + count, sizeof(buffer) - count, "*%02X\r\n", checksum((unsigned char *)buffer, count));
	endpos = count;
	buffer[endpos + 1] = '\0';

	// step2. 发送
	pkt = response_pkt_new(((p2pclient_t)(g_instapp->p2p_frontsvr))->pktpool, endpos + 1);
	memcpy(pkt->data, buffer, endpos + 1);
	pkt->len = endpos + 1;
	p2pclient_write((p2pclient_t)(g_instapp->p2p_frontsvr), pkt);

	Seqno2FrontsvrManage();
}




extern char * slice_str(char * src, char start, char end, char buffer[], int bufflen);

int explain_bsi_my(const char byFrame[], int nFrameLen, int *bdid, const char *DataBuff, DWORD &Len)
{
	//int bdid = 0;
	char power[30];
	char * pstr = NULL;

	tagBeamInfo* pBeamInfo = (tagBeamInfo*)DataBuff;
	memset(pBeamInfo, 0, sizeof(tagBeamInfo));

	// $BDBSI,1234([1,1,0,2,5,4,2,5,1,8])A4*2F

	// pstr = ",1234([1,1,0,2,5,4,2,5,1,8])A4*2F"
	pstr = strstr((char*)byFrame, ",");
	NULLReturn(pstr, CUNPACK_NULL);
	if (pstr[1] != ',')
	 {
		 if (CheckInt(pstr + 1) == FALSE)
			 return CUNPACK_NULL;
		 *bdid = atoi(&pstr[1]);
		 //_itoa(bdid,(char*)pBeamInfo->byResponseNo,10);
		 if (bdid <= 0)
			 return CUNPACK_NULL;
	 }

	//// pstr = "([1,1,0,2,5,4,2,5,1,8]),A4*2F"
	//pstr = strstr(&pstr[1], "(");


	slice_str((char*)byFrame, '(', ')', power, sizeof(power));
	memcpy(pBeamInfo->byPower, power, sizeof(power));

	Len = sizeof(tagBeamInfo);
	return CUNPACK_BSI;
}



/* BDBSI消息解析函数：设备状态信息 */
static int explain_bsi(const char byFrame[], int nFrameLen, const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	tagBeamInfo* pBeamInfo = (tagBeamInfo*)DataBuff;
	memset(pBeamInfo, 0, sizeof(tagBeamInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{
		if (CheckInt(f_szTemp2 + 1) == FALSE)
			return CUNPACK_NULL;
		pBeamInfo->byResponseNo = atoi(&f_szTemp2[1]);
		if (!CheckCloseRange(pBeamInfo->byResponseNo, 1, 10))
			return CUNPACK_NULL;
	}
	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{
		if (CheckInt(f_szTemp2 + 1) == FALSE)
			return CUNPACK_NULL;
		pBeamInfo->byTimeDiffNo = atoi(&f_szTemp2[1]);
		if (!CheckCloseRange(pBeamInfo->byTimeDiffNo, 1, 10))
			return CUNPACK_NULL;
	}

	for (int i = 0; i < 5; i++)
	{
		f_szTemp2 = strstr(&f_szTemp2[1], ",");
		NULLReturn(f_szTemp2, CUNPACK_NULL);
		BOOL bCheck = CheckDouble(f_szTemp2 + 1);
		FALSEReturn(bCheck, CUNPACK_NULL);

		if (f_szTemp2[1] != ',')
		{
			pBeamInfo->byPower[i * 2] = atoi(&f_szTemp2[1]);
			if (!CheckCloseRange(pBeamInfo->byPower[i * 2], 0, 4))
				return CUNPACK_NULL;
		}

		f_szTemp2 = strstr(&f_szTemp2[1], ",");
		NULLReturn(f_szTemp2, CUNPACK_NULL);

		bCheck = CheckDouble(f_szTemp2 + 1);
		FALSEReturn(bCheck, CUNPACK_NULL);

		if (f_szTemp2[1] != ',' && f_szTemp2[1] != '*')
		{
			pBeamInfo->byPower[i * 2 + 1] = atoi(&f_szTemp2[1]);

			if (!CheckCloseRange(pBeamInfo->byPower[i * 2 + 1], 0, 4))
				return CUNPACK_NULL;
		}
	}
	Len = sizeof(tagBeamInfo);
	return CUNPACK_BSI;
}


/* BDWAA消息解析函数：接收到子用户的位置报告 */
static int explain_waa(const char byFrame[], int nFrameLen, const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	char* f_szTemp1 = NULL;
	f_szTemp2 = NULL;
	string  strTemp;
	double  dfData = 0.0;
	lpWAAInfo pWAAInfo = (lpWAAInfo)DataBuff;
	memset(pWAAInfo, 0, sizeof(WAAInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{
		if (CheckInt(f_szTemp2 + 1) == FALSE)
			return CUNPACK_NULL;//caution
		pWAAInfo->byInfoType = atoi(&f_szTemp2[1]);
		if (CheckCloseRange(pWAAInfo->byInfoType, 0, 1) != TRUE)
			return CUNPACK_NULL;
	}

	f_szTemp2 = strstr((char*)f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{
		if (CheckDouble(f_szTemp2 + 1) == FALSE)
			return CUNPACK_NULL;
		pWAAInfo->nReportFreq = atof(&f_szTemp2[1]);
	}
	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2 + 1);
		FALSEReturn(bCheck, CUNPACK_NULL);
		pWAAInfo->nUserId = atoi(&f_szTemp2[1]);
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{

		BOOL bCheck = FALSE;
		int  nFieldLen = FieldLength(f_szTemp2 + 1);//
		bCheck = (nFieldLen > 5);
		FALSEReturn(bCheck, CUNPACK_NULL);

		if (CheckDouble(f_szTemp2 + 1) == FALSE)
			return CUNPACK_NULL;

		if (nFieldLen == 6)
			FALSEReturn(CheckInt(f_szTemp2 + 1), CUNPACK_NULL);

		strTemp = f_szTemp2 + 1;//caution
		pWAAInfo->wReportHour = atoi(strTemp.substr(0, 2).c_str());
		pWAAInfo->wReportHour = (pWAAInfo->wReportHour + 8) % 24;
		pWAAInfo->wReportMinute = atoi(strTemp.substr(2, 2).c_str());
		const char * pstr = strTemp.c_str();
		pWAAInfo->fReportSecond = atof(pstr + 4);

		bCheck = CheckCloseRange(pWAAInfo->wReportHour, 0, 23);
		FALSEReturn(bCheck, CUNPACK_NULL);

		bCheck = CheckCloseRange(pWAAInfo->wReportMinute, 0, 59);
		FALSEReturn(bCheck, CUNPACK_NULL);

		bCheck = CheckLCloseRange(pWAAInfo->fReportSecond, 0, 60);
		FALSEReturn(bCheck, CUNPACK_NULL);

	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2 + 1);
		FALSEReturn(bCheck, CUNPACK_NULL);
		FALSEReturn(IntpartLen(f_szTemp2 + 1) == 4, CUNPACK_NULL);

		strTemp = f_szTemp2 + 1;

		pWAAInfo->dfLat = atoi(strTemp.substr(0, 2).c_str());
		const char* pstr = strTemp.c_str();
		dfData = atof(pstr + 2);
		FALSEReturn(CheckLCloseRange(dfData, 0, 60), CUNPACK_NULL);//caution
		pWAAInfo->dfLat += dfData / 60;

		bCheck = CheckCloseRange(pWAAInfo->dfLat, 0, 90);
		FALSEReturn(bCheck, CUNPACK_NULL);
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);//caution
	if (f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2 + 1) == 1);//
		FALSEReturn(bCheck, CUNPACK_NULL);
		FALSEReturn(f_szTemp2[1] == 'N' || f_szTemp2[1] == 'S', CUNPACK_NULL);
		pWAAInfo->chLatDir = *(f_szTemp2 + 1);
	}
	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{//
		BOOL bCheck = CheckDouble(f_szTemp2 + 1);//
		FALSEReturn(bCheck, CUNPACK_NULL);//
		FALSEReturn(IntpartLen(f_szTemp2 + 1) == 5, CUNPACK_NULL);//

		strTemp = f_szTemp2 + 1;//11420.736
		pWAAInfo->dfLong = atoi(strTemp.substr(0, 3).c_str());//
		const char * pstr = strTemp.c_str();//caution
		dfData = atof(pstr + 3);//
		FALSEReturn(CheckLCloseRange(dfData, 0, 60), CUNPACK_NULL);//
		pWAAInfo->dfLong += dfData / 60;//
		//
		bCheck = CheckLCloseRange(pWAAInfo->dfLong, 0, 180);//
		FALSEReturn(bCheck, CUNPACK_NULL);//
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2 + 1) == 1);//
		FALSEReturn(bCheck, CUNPACK_NULL);
		FALSEReturn(f_szTemp2[1] == 'W' || f_szTemp2[1] == 'E', CUNPACK_NULL);
		pWAAInfo->chLongDir = (f_szTemp2[1]);
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{
		if (CheckDouble(f_szTemp2 + 1) == FALSE)
			return CUNPACK_NULL;
		pWAAInfo->dfHgt = atof(f_szTemp2 + 1);
	}
	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);//caution

	if (f_szTemp2[1] != ','&&f_szTemp2[1] != '*')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2 + 1) == 1);//
		FALSEReturn(bCheck, CUNPACK_NULL);

		pWAAInfo->chHgtUnit = (f_szTemp2[1]);
	}

	Len = sizeof(WAAInfo);

	return CUNPACK_WAA;
}



/* BDTXR消息解析函数：监收通信 */
static int explain_txr(const char byFrame[], int nFrameLen, const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	string strTime;
	DWORD  nBitLen = 0;

	lpTXRInfo pTXRInfo = (lpTXRInfo)DataBuff;
	memset(pTXRInfo, 0, sizeof(TXRInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");

	NULLReturn(f_szTemp2, CUNPACK_NULL);

	if (f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2 + 1);
		FALSEReturn(bCheck, CUNPACK_NULL);
		pTXRInfo->byInfoType = atoi(&f_szTemp2[1]);
		if (CheckCloseRange(pTXRInfo->byInfoType, 1, 5) != TRUE)//caution
			return CUNPACK_NULL;
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2 + 1);
		FALSEReturn(bCheck, CUNPACK_NULL);
		pTXRInfo->nLocalID = atoi(&f_szTemp2[1]);
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{//caution
		BOOL bCheck = CheckInt(f_szTemp2 + 1);
		FALSEReturn(bCheck, CUNPACK_NULL);
		pTXRInfo->byTeleFormat = atoi(&f_szTemp2[1]);
		if (CheckCloseRange(pTXRInfo->byTeleFormat, 0, 2) != TRUE)
			return CUNPACK_NULL;
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		strTime = f_szTemp2 + 1;
		if (CheckInt(f_szTemp2 + 1) == FALSE)
			return CUNPACK_NULL;

		int FieldLen = FieldLength(f_szTemp2 + 1);//

		if (FieldLen == 4)
		{
			pTXRInfo->bySendHour = atoi(strTime.substr(0, 2).c_str());
			bCheck = CheckCloseRange(pTXRInfo->bySendHour, 0, 23);
			FALSEReturn(bCheck, CUNPACK_NULL);//caution

			pTXRInfo->bySendMinute = atoi(strTime.substr(2, 2).c_str());
			bCheck = CheckCloseRange(pTXRInfo->bySendMinute, 0, 59);
			FALSEReturn(bCheck, CUNPACK_NULL);
		}
		else
		{
			pTXRInfo->bySendHour = 0;
			pTXRInfo->bySendMinute = 0;
		}
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{
		f_szTemp2++;

		int i = 0;
		for (i = 0; i < sizeof(pTXRInfo->chContent) - 1; i++)
		{
			if (f_szTemp2[i - 4] == '*'&&f_szTemp2[i - 1] == '\r' &&f_szTemp2[i] == '\n')
				break;
		}

		memcpy(pTXRInfo->chContent, f_szTemp2, i - 4);
		pTXRInfo->chContent[i - 4] = '\0';
		pTXRInfo->nContentLen = i - 4;
	}

	if (pTXRInfo->byTeleFormat == 1)
	{
		int j = 0;
		for (; j < pTXRInfo->nContentLen; j++)
		{
			if (!IsHex(pTXRInfo->chContent[j]))
				break;
		}

		TRUEReturn(j < pTXRInfo->nContentLen, CUNPACK_NULL);
	}

	if (pTXRInfo->byTeleFormat == 2)
	{
		TRUEReturn(pTXRInfo->nContentLen < 2, CUNPACK_NULL);
		if (pTXRInfo->chContent[0] != 'A' || pTXRInfo->chContent[1] != '4')
			return CUNPACK_NULL;

		/*char chSet[4] = { '0', '2', '3', '4' };
		BOOL bCheck = CheckInSet(pTXRInfo->chContent[1], chSet, 4);
		if (!bCheck)
			return CUNPACK_NULL;*/

		// 此处开始，是为扩展协议而添加
		if ((pTXRInfo->chContent[2] != 'B') || (pTXRInfo->chContent[3] != 'F'))
			return CUNPACK_NULL;
			
		/*if ((pTXRInfo->chContent[2] != '4') || (pTXRInfo->chContent[3] != '2'))
			return CUNPACK_NULL;*/


		// 如果第5个字符是0，则表示告警信息，需要根据第6个字符来确定具体的告警类型
		if (pTXRInfo->chContent[4] == '0')
		{
			if (pTXRInfo->chContent[5] == '0')
				pTXRInfo->EventType = warn_low_power;
			if (pTXRInfo->chContent[5] == '1')
				pTXRInfo->EventType = warn_destroy;

			Len = sizeof(TXRInfo);
			return CUNPACK_TXR;
		}
		// 如果第5个字符是1，则表示开锁请求，后面第6个字符应为固定的0，然后紧接着是timestamp+digest串
		else if (pTXRInfo->chContent[4] == '1' && pTXRInfo->chContent[5] == '0')
		{
			pTXRInfo->EventType = req_unlock;

			//char timebuffer[14];
			memcpy(pTXRInfo->time, (char*)(&pTXRInfo->chContent[6]), 14);
			//memcpy_s(timebuffer, sizeof(timebuffer), (char*)(&pTXRInfo->chContent[6]), 14);
			//pTXRInfo->time = timebuffer;
			pTXRInfo->time[15] = '\0';
			
			//char *digestbuffer = NULL;
			memcpy(pTXRInfo->digest, (char*)(&pTXRInfo->chContent[20]), 32);
			//memcpy_s(digestbuffer, sizeof(digestbuffer), (char*)(&pTXRInfo->chContent[20]), 32);
			//pTXRInfo->digest = digestbuffer;
			pTXRInfo->digest[33] = '\0';

		}
		// 如果第5个字符是2，则表示请求，根据第6个字符来判断请求类型为任务暂停或者恢复请求
		else if (pTXRInfo->chContent[4] == '2')
		{
			if (pTXRInfo->chContent[5] == '0')
				pTXRInfo->EventType = req_taskpau;
			if (pTXRInfo->chContent[5] == '1')
				pTXRInfo->EventType = req_taskcon;

			char buffer[512];	// 用来存储请求附带的描述信息
			// AscToHex
			int bits = hexchars_bytes((unsigned char*)buffer, sizeof(buffer), (char *)(pTXRInfo->chContent + 6), pTXRInfo->nContentLen - 6);
			memcpy_s((unsigned char*)pTXRInfo->chContent, sizeof(pTXRInfo->chContent), (unsigned char*)buffer, bits / 8 + (bits % 8 ? 1 : 0));
			pTXRInfo->nContentLen = bits / 8 + (bits % 8 ? 1 : 0);
			pTXRInfo->chContent[pTXRInfo->nContentLen] = '\0';
		}
		else
			pTXRInfo->EventType = unknown;

		//pTXRInfo->nDestionID = 0;		// 后期需要更改：暂时置零

		Len = sizeof(TXRInfo);
		return CUNPACK_TXR;
	}
}


/* BDDWR消息解析函数：监听子用户定位信息 */
static int explain_dwr(const char byFrame[], int nFrameLen, const char *DataBuff, DWORD &Len)
{

	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	double dfData;
	int    nTemp;
	std::string strTemp;
	char * pstr = NULL;
	std::string strData;

	lpDWRInfo pDWRInfo = (lpDWRInfo)DataBuff;
	memset(pDWRInfo, 0, sizeof(DWRInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{
		pDWRInfo->nPosType = atoi(f_szTemp2 + 1);
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{
		pDWRInfo->nLocalId = atoi(&f_szTemp2[1]);
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{
		strData = (f_szTemp2 + 1);
		pDWRInfo->nPosHour = atoi(strData.substr(0, 2).c_str());
		// pDWRInfo->nPosHour = (pDWRInfo->nPosHour + 8) % 24;
		pDWRInfo->nPosHour = (pDWRInfo->nPosHour) % 24;
		pDWRInfo->nPosMinute = atoi(strData.substr(2, 2).c_str());
		const char* pstr1 = strData.c_str();
		pDWRInfo->fPosSecond = atof(pstr1 + 4);
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);

	if (f_szTemp2[1] != ',')
	{
		strData = f_szTemp2 + 1;
		BOOL bCheck = FALSE;
		strTemp = strData.substr(0, 2);
		nTemp = atoi(strTemp.c_str());

		const char* pstr1 = strData.c_str();

		dfData = atof(pstr1 + 2);
		dfData = dfData / 60 + nTemp;
		pDWRInfo->dfLat = dfData;
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{//caution
		BOOL bCheck = FALSE;
		pDWRInfo->chLatDirection = f_szTemp2[1];
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{
		strData = f_szTemp2 + 1;
		nTemp = atoi(strData.substr(0, 3).c_str());
		const char* pstr1 = strData.c_str();
		dfData = atof(pstr1 + 3);
		dfData = dfData / 60 + nTemp;
		pDWRInfo->dfLong = dfData;
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);

	if (f_szTemp2[1] != ',')
	{
		pDWRInfo->chLongDirection = f_szTemp2[1];
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{//caution
		pDWRInfo->fEarthHgt = atof(&f_szTemp2[1]);
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{
		pDWRInfo->chHgtUnit = f_szTemp2[1];
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);//caution
	if (f_szTemp2[1] != ',')
	{
		pDWRInfo->dfHgtOut = atof(&f_szTemp2[1]);
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{
		pDWRInfo->chHgtOutUnit = f_szTemp2[1];
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{//caution
		pDWRInfo->byPrecisionInd = atoi(&f_szTemp2[1]);
	}
	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);

	if (f_szTemp2[1] != ',')
	{
		if (f_szTemp2[1] == 'A')
			pDWRInfo->bUrgent = TRUE;//caution
		else
			pDWRInfo->bUrgent = FALSE;
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ',')
	{
		if (f_szTemp2[1] == 'A')
			pDWRInfo->bMultiValue = TRUE;
		else
			pDWRInfo->bMultiValue = FALSE;
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2, CUNPACK_NULL);
	if (f_szTemp2[1] != ','&&f_szTemp2[1] != '*')
	{//caution
		if (f_szTemp2[1] == 'H')
			pDWRInfo->bGaoKong = TRUE;
		else
			pDWRInfo->bGaoKong = FALSE;
	}
	Len = sizeof(DWRInfo);
	return CUNPACK_DWR;
}

static double CalLon(unsigned int Hour, unsigned int Min, unsigned int Sec, unsigned int MS)
{
	double LonRslt = 0;

	LonRslt = Hour + (60.0 * Min + Sec + MS / 10.0) / 3600.0;

	return LonRslt;
}

static double CalLat(unsigned int Hour, unsigned int Min, unsigned int Sec, unsigned int MS)
{
	double LatRslt = 0;

	LatRslt = Hour + (60 * Min + Sec + MS / 10.0) / 3600.0;

	return LatRslt;
}


static char * get_formatted_time(char *strtime)
{
	if (strtime == NULL)
		return NULL;
	char time[20];
	char *formatted_time;
	char year[4];
	char month[2];
	char day[2];
	char hour[2];
	char minute[2];
	char second[2];

	memcpy(year, strtime, sizeof(year));
	memcpy(month, strtime+4, sizeof(month));
	memcpy(day, strtime+6, sizeof(day));
	memcpy(hour, strtime+8, sizeof(hour));
	memcpy(minute, strtime+10, sizeof(minute));
	memcpy(second, strtime+12, sizeof(second));

	sprintf(time, "%04d-%02d-%02d %02d:%02d:%02d", atoi(year),atoi(month),atoi(day),atoi(hour),atoi(minute),atoi(second));

	formatted_time = _strdup(time);
	//formatted_time[20] = '\0';

	return formatted_time;
}

//收到bdjobber发送的数据，回复给bdjobber，或者转发给frontsvr
void  bdjobber_exp_cb(int msgid, void* msg, int len, void* param)
{
	int  msgcode = 0;
	unsigned char* frame = (unsigned char*)msg;
	if (len > 70)
	{
		int j = 0;
		j++;
	
	}
	bdjobber_exp_t exp = (bdjobber_exp_t)param;
	sess_t localsess = (sess_t)_BASE(exp).param;
	p2p_t p2p = (p2p_t)localsess;
	/*p2pclient_t p2pclient = (p2pclient_t)localsess;*/

	if (exp == NULL || localsess == NULL)
	{
		int i = 0;
		i++;
		return;
	}

	if (localsess->type != bdjobber_SESS){
		p2p_kill(p2p); return;
	}

	switch (msgid)
	{
		case cunpack_ping:	// 针对来自bdjobber的ping请求，回复pong消息
		{
			log_write(g_instapp->log, LOG_NOTICE, "recv ping from %s.", localsess->sname);
			protobuffer_svr_send(p2p, eProto_pong, localsess->sname);
			log_write(g_instapp->log, LOG_NOTICE, "send pong to %s.", localsess->sname);
		}
		break;

		case cunpack_jobme:	// 针对来自bdjobber的jobme请求，当无任务时，回复nojob消息
		{
			log_write(g_instapp->log, LOG_NOTICE, "recv <JOBME> from %s.", localsess->sname);

			jobber_free = TRUE;
			log_write(g_instapp->log, LOG_NOTICE, "set jobber_free to [TRUE].");

			if (jqueue_size(g_instapp->jobqueue) == 0)
			{
				protobuffer_svr_send(p2p, eProto_nojob, localsess->sname);	// 系统空闲,回复nojob
				log_write(g_instapp->log, LOG_NOTICE, "send <NOJOB> to bdjob",localsess->sname);
			}		
		}
		break;

		case cunpack_txr:		// 收到的事件信息：warn与reqcode
		{
			//unsigned char * testframe = "$BDTXR,1,195817,2,1439,A4BF0113ECCB13ECCBHELLO*49";
		
			char Destbuffer[2048] = { 0 };
			DWORD imsglen = 0;
			explain_txr((const char*)frame, len, Destbuffer, imsglen);		// 2.1协议解析来自bdjobber的消息
			evtreport_t bdtxr_tmp = NULL;
			bdtxr_tmp = (evtreport_t)calloc(1, sizeof(evtreport_st));
			if (NULL == bdtxr_tmp)
			{
				log_debug(ZONE, "error: bddwr_tmp calloc fail.");
				return;
			}

			lpTXRInfo pTXRInfo = (lpTXRInfo)Destbuffer;

			bdtxr_tmp->bdid = pTXRInfo->nLocalID;		// int类型
			if (bdtxr_tmp->bdid == 0)
			{
				free(bdtxr_tmp);
				log_debug(ZONE, "error: From ID = 0.");
				return;
			}
			// 根据北斗号查询对应的taskid、hid与executor
			char *hid = NULL;
			char *taskid = NULL;
			char *executor = NULL;
			char bdid[7] = { 0 };
			_itoa(bdtxr_tmp->bdid, bdid, 10);		// int类型bdtxr_tmp->bdid转为char *类型的bdid
			if (-1 == db_get_hid_taskid_executor_by_bdid((mysqlquery_t)g_instapp->sqlobj, bdid, &taskid, &hid,&executor))
			{
				free(bdtxr_tmp);
				printf("db_get_hid_taskid_executor_by_bdid fail.\r\n");
				return;
			}
			if (taskid == NULL || hid == NULL || executor == NULL)
			{
				free(bdtxr_tmp);
				printf("taskid OR hid OR executor = null.\r\n");
				return;
			}

			bdtxr_tmp->taskid = taskid;
			bdtxr_tmp->hid = hid;
			//bdtxr_tmp->executor = executor;
			bdtxr_tmp->executor = bdid;
			bdtxr_tmp->evt_type = pTXRInfo->EventType;

			// 如果类型是告警信息，则拼成$warn串，发给frontsvr
			if (bdtxr_tmp->evt_type == warn_low_power)
			{
				bdtxr_tmp->time = get_formatted_time(pTXRInfo->time);
				bdtxr_tmp->digest = pTXRInfo->digest;

				warn2frontsvr(bdtxr_tmp, warn_low_power);
				log_write(g_instapp->log, LOG_NOTICE, "send --warn_low_power-- to frontsvr.");
				free(bdtxr_tmp->time);
			}

			// 如果类型是开锁请求，则拼成$reqcode串，发给frontsvr
			else if (bdtxr_tmp->evt_type == req_unlock)
			{
				bdtxr_tmp->time = pTXRInfo->time;
				bdtxr_tmp->digest = pTXRInfo->digest;

				reqcode2frontsvr(bdtxr_tmp);
				log_write(g_instapp->log, LOG_NOTICE, "send --unlock req-- to frontsvr.");
				//free(bdtxr_tmp->time);
			}

			// 如果类型是任务暂停请求，则拼成$taskpau串，发给frontsvr
			else if (bdtxr_tmp->evt_type == req_taskpau)
			{
				bdtxr_tmp->time = get_formatted_time(pTXRInfo->time);
				bdtxr_tmp->digest = pTXRInfo->digest;
				bdtxr_tmp->content = pTXRInfo->chContent;

				taskpau2frontsvr(bdtxr_tmp);
				log_write(g_instapp->log, LOG_NOTICE, "send --taskpau req-- to frontsvr.");
				free(bdtxr_tmp->time);
			}

			// 如果类型是任务恢复请求，则拼成$taskcon串，发给frontsvr
			else if (bdtxr_tmp->evt_type == req_taskcon)
			{
				bdtxr_tmp->time = get_formatted_time(pTXRInfo->time);
				bdtxr_tmp->digest = pTXRInfo->digest;
				bdtxr_tmp->content = pTXRInfo->chContent;

				taskcon2frontsvr(bdtxr_tmp);
				log_write(g_instapp->log, LOG_NOTICE, "send --taskcon req-- to frontsvr.");
				free(bdtxr_tmp->time);
			}

			else
			{
				
			}

			free(bdtxr_tmp);
			bdtxr_tmp = NULL;
		}
		break;

		case cunpack_dwr:	// 位置报告
		{
			char ctimetemp[64] = { 0 };
			char Destbuffer[2048] = { 0 };
			DWORD imsglen = 0;		
			
			explain_dwr((const char*)frame, len, Destbuffer, imsglen);		// 根据2.1协议解析来自bdjobber的消息
			lpDWRInfo pDWRInfo = (lpDWRInfo)Destbuffer;

			bdposreport_t bddwr_tmp = NULL;
			bddwr_tmp = (bdposreport_t)calloc(1, sizeof(bdposreport_st));
			if (NULL == bddwr_tmp)
			{
				free(bddwr_tmp);
				log_debug(ZONE, "error: bddwr_tmp calloc fail..");
				return;
			}
			
			bddwr_tmp->bdid = pDWRInfo->nLocalId;	//子用户的北斗号
			if (bddwr_tmp->bdid == 0)
			{
				log_debug(ZONE, "error: From ID = 0.");
				return;
			}
			// 根据北斗号查询对应的taskid
			char *hid = NULL;
			char *taskid = NULL;
			char *executor = NULL;
			char bdid[7] = { 0 };
			_itoa(bddwr_tmp->bdid, bdid, 10);
			if (-1 == db_get_hid_taskid_executor_by_bdid((mysqlquery_t)g_instapp->sqlobj, bdid, &taskid, &hid, &executor))
			{
				free(bddwr_tmp);
				printf("db_get_hid_taskid_by_bdid fail.\r\n");
				return;
			}
			if (taskid == NULL || hid == NULL || executor == NULL)
			{
				free(bddwr_tmp);
				printf("taskid OR hid OR executor = null.\r\n");
				return;
			}

			bddwr_tmp->taskid = taskid;
			bddwr_tmp->executor = executor;
			bddwr_tmp->longtitue = pDWRInfo->dfLong;
			bddwr_tmp->lattitude = pDWRInfo->dfLat;
			bddwr_tmp->height = pDWRInfo->fEarthHgt;

			SYSTEMTIME T;
			GetLocalTime(&T);
			int nSecond = (int)(pDWRInfo->fPosSecond + 0.5);	
			sprintf(bddwr_tmp->time, "%04d-%02d-%02d %02d:%02d:%02d", T.wYear, T.wMonth, T.wDay, T.wHour, T.wMinute, T.wSecond);

			if (pDWRInfo->nPosType == 3) 
			{
				pos2frontsvr(bddwr_tmp);
				log_write(g_instapp->log, LOG_NOTICE, "send --bdposition-- to frontsvr.");
			}

			free(bddwr_tmp);
			bddwr_tmp = NULL;	
		}
			break;

		case CUNPACK_BSI:		// 接收到设备状态信息
		{
			log_debug(ZONE, "recv $BDBSI from bdjobber...[%s].",frame);

			char ctimetemp[64] = { 0 };
			char  * report_json = NULL;			// 设备状态上报的json串部分
			char Destbuffer[2048] = { 0 };
			DWORD imsglen = 0;
			int bdid = 0;
			//explain_bsi((const char*)frame, len, Destbuffer, imsglen);		// 2.1协议解析来自bdjobber的消息
			explain_bsi_my((const char*)frame, len, &bdid, Destbuffer, imsglen);		// 2.1协议解析来自bdjobber的消息

			bsireport_t bdbsi_tmp = NULL;
			bdbsi_tmp = (bsireport_t)calloc(1, sizeof(bsireport_st));
			if (NULL == bdbsi_tmp)  return;
			tagBeamInfo* pBeamInfo = (tagBeamInfo*)Destbuffer;

			bdbsi_tmp->bdid = bdid;	
			if (bdbsi_tmp->bdid == 0)
			{
				free(bdbsi_tmp);
				log_debug(ZONE, "error: From ID = 0.");
				return;
			}
			//bdbsi_tmp->state = pBeamInfo->byPower;
			memcpy(bdbsi_tmp->state, pBeamInfo->byPower, sizeof(pBeamInfo->byPower));
			SYSTEMTIME T;
			GetLocalTime(&T);
			sprintf(ctimetemp, "%04d-%02d-%02d %02d:%02d:%02d", T.wYear, T.wMonth, T.wDay, T.wHour, T.wMinute, T.wSecond);
			strcpy(bdbsi_tmp->time, ctimetemp);

			report_json = bsireport_jsonstring(bdbsi_tmp);
			//submit2smcache(report_json);
			log_write(g_instapp->log, LOG_NOTICE, "send [bdbsi] to frontsvr.");

			free(report_json);
			free(bdbsi_tmp);
			bdbsi_tmp = NULL;
		}
		break;

		/*
		case CUNPACK_WAA:		// 接收到子用户的位置报告
		{
			char ctimetemp[64] = { 0 };
			char  * report_json = NULL;			// 位置上报的json串部分
			char Destbuffer[2048] = { 0 };
			DWORD imsglen = 0;
			explain_waa((const char*)frame, len, Destbuffer, imsglen);		// 2.1协议解析来自bdjobber的消息

			bdposreport_t bdwaa_tmp = NULL;
			bdwaa_tmp = (bdposreport_t)calloc(1, sizeof(bdposreport_st));
			if (NULL == bdwaa_tmp)  return;
			lpWAAInfo pWAAInfo = (lpWAAInfo)Destbuffer;

			bdwaa_tmp->user_id = pWAAInfo->nUserId;	//子用户的北斗号
			if (bdwaa_tmp->user_id == 0)
			{
				log_debug(ZONE, "error: From ID = 0.");
				return;
			}
			bdwaa_tmp->longtitue = pWAAInfo->dfLong;
			bdwaa_tmp->lattitude = pWAAInfo->dfLat;
			bdwaa_tmp->height = pWAAInfo->dfHgt;
			SYSTEMTIME T;
			GetLocalTime(&T);
			int nSecond = (int)(pWAAInfo->fReportSecond + 0.5);
			sprintf(ctimetemp, "%04d-%02d-%02d %02d:%02d:%02d", T.wYear, T.wMonth, T.wDay, pWAAInfo->wReportHour, pWAAInfo->wReportMinute, nSecond);
			strcpy(bdwaa_tmp->time, ctimetemp);

			report_json = posreport_jsonstring(bdwaa_tmp);
			submit2smcache(report_json);

			free(bdwaa_tmp);
			bdwaa_tmp = NULL;
		}
		break;
		*/

		default:
			break;
	}
}
