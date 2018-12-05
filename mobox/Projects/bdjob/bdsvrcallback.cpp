#include "bdsvrcallback.h"
#include "p2p.h"
#include "protobuffer.h"
#include "expat_bdsvr.h"
#include "instapp.h"
#include "tool.h"

//template<typename T, typename TS>
//BOOL CheckCloseRange(T nData, TS nStart, TS nEnd)
//{
//	return (nData >= nStart && nData <= nEnd);
//}
//
//template<typename T, typename TS>
//BOOL CheckInSet(T data, TS Set[], int nSetSize)
//{
//	BOOL bRtn = FALSE;
//
//	if (Set != NULL && nSetSize > 0)
//	{
//		int i = 0;
//		for (; i < nSetSize; i++)
//			if (data == Set[i])
//			{
//				bRtn = TRUE;
//				break;
//			}
//	}
//
//	return bRtn;
//}
//
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
int AscToHex(unsigned char *desthexbuff, unsigned int destbufflen, char *srcbuff, unsigned int srclen)
{
	unsigned char data;
	int rtn = 0;

	for (size_t i = 0; i < srclen; i++){
		__char_hex(srcbuff[i], data);
		if (i % 2 == 0){
			desthexbuff[i / 2] = data << 4;
			rtn += 4;
		}
		else{
			desthexbuff[i / 2] |= data;
			rtn += 4;
		}

		if (rtn == destbufflen * 8){
			rtn = destbufflen * 8;
			break;
		}
	}

	return rtn;
}
int  hexchars_bytes(unsigned char destbuffer[], int destbufflen, char srcbuffer[], int srclen){
	return AscToHex(destbuffer, destbufflen, srcbuffer, srclen);
}
//BOOL   IsHex(char chData)
//{
//	if (ISDIGIT(chData) || (chData >= 'A'&&chData <= 'F') || (chData >= 'a'&&chData <= 'f'))
//		return TRUE;
//	else
//		return FALSE;
//}
//
//BOOL CheckInt(const char* pstr)
//{
//	BOOL   bRtn = FALSE;
//	char * pEnd = NULL;
//
//	int nData = strtol(pstr, &pEnd, 10);
//	if (*pEnd == ',' || *pEnd == '\0' || *pEnd == '*')
//		bRtn = TRUE;
//
//	return bRtn;
//}
//
//int  FieldLength(const char* str)
//{
//	int i = 0;
//	for (; (*str) != NULL && (*str != ',') && (*str) != '*'; i++)
//	{
//		str++;
//		if (i == 1024)//长度没有大于1024的字段。
//			break;
//	}
//
//	return i;
//}
//static int explain_txr(const char byFrame[], int nFrameLen, const char *DataBuff, DWORD &Len)
//{
//	int  f_iInfoID = 0;
//	int  f_iAppendIndex = 0;
//	char *f_szTemp2 = NULL;
//	int  iTitleIter = 0;
//
//	string strTime;
//	DWORD  nBitLen = 0;
//
//	lpTXRInfo pTXRInfo = (lpTXRInfo)DataBuff;
//	memset(pTXRInfo, 0, sizeof(TXRInfo));
//
//	f_szTemp2 = strstr((char*)byFrame, ",");
//
//	NULLReturn(f_szTemp2, CUNPACK_NULL);
//
//	if (f_szTemp2[1] != ',')
//	{
//		BOOL bCheck = CheckInt(f_szTemp2 + 1);
//		FALSEReturn(bCheck, CUNPACK_NULL);
//		pTXRInfo->byInfoType = atoi(&f_szTemp2[1]);
//		if (CheckCloseRange(pTXRInfo->byInfoType, 1, 5) != TRUE)//caution
//			return CUNPACK_NULL;
//	}
//
//	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
//	NULLReturn(f_szTemp2, CUNPACK_NULL);
//	if (f_szTemp2[1] != ',')
//	{
//		BOOL bCheck = CheckInt(f_szTemp2 + 1);
//		FALSEReturn(bCheck, CUNPACK_NULL);
//		pTXRInfo->nLocalID = atoi(&f_szTemp2[1]);
//	}
//
//	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
//	NULLReturn(f_szTemp2, CUNPACK_NULL);
//	if (f_szTemp2[1] != ',')
//	{//caution
//		BOOL bCheck = CheckInt(f_szTemp2 + 1);
//		FALSEReturn(bCheck, CUNPACK_NULL);
//		pTXRInfo->byTeleFormat = atoi(&f_szTemp2[1]);
//		if (CheckCloseRange(pTXRInfo->byTeleFormat, 0, 2) != TRUE)
//			return CUNPACK_NULL;
//	}
//
//	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
//	NULLReturn(f_szTemp2, CUNPACK_NULL);
//	if (f_szTemp2[1] != ',')
//	{
//		BOOL bCheck = FALSE;
//		strTime = f_szTemp2 + 1;
//		if (CheckInt(f_szTemp2 + 1) == FALSE)
//			return CUNPACK_NULL;
//
//		int FieldLen = FieldLength(f_szTemp2 + 1);//
//
//		if (FieldLen == 4)
//		{
//			pTXRInfo->bySendHour = atoi(strTime.substr(0, 2).c_str());
//			bCheck = CheckCloseRange(pTXRInfo->bySendHour, 0, 23);
//			FALSEReturn(bCheck, CUNPACK_NULL);//caution
//
//			pTXRInfo->bySendMinute = atoi(strTime.substr(2, 2).c_str());
//			bCheck = CheckCloseRange(pTXRInfo->bySendMinute, 0, 59);
//			FALSEReturn(bCheck, CUNPACK_NULL);
//		}
//		else
//		{
//			pTXRInfo->bySendHour = 0;
//			pTXRInfo->bySendMinute = 0;
//		}
//	}
//
//	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
//	NULLReturn(f_szTemp2, CUNPACK_NULL);
//	if (f_szTemp2[1] != ',')
//	{
//		f_szTemp2++;
//
//		int i = 0;
//		for (i = 0; i < sizeof(pTXRInfo->chContent) - 1; i++)
//		{
//			if (f_szTemp2[i - 4] == '*'&&f_szTemp2[i - 1] == '\r' &&f_szTemp2[i] == '\n')
//				break;
//		}
//
//		memcpy(pTXRInfo->chContent, f_szTemp2, i - 4);
//		pTXRInfo->chContent[i - 4] = '\0';
//		pTXRInfo->nContentLen = i - 4;
//	}
//
//	if (pTXRInfo->byTeleFormat == 1)
//	{
//		int j = 0;
//		for (; j < pTXRInfo->nContentLen; j++)
//		{
//			if (!IsHex(pTXRInfo->chContent[j]))
//				break;
//		}
//
//		TRUEReturn(j < pTXRInfo->nContentLen, CUNPACK_NULL);
//	}
//
//	if (pTXRInfo->byTeleFormat == 2)
//	{
//		TRUEReturn(pTXRInfo->nContentLen < 2, CUNPACK_NULL);
//		if (pTXRInfo->chContent[0] != 'A')
//			return CUNPACK_NULL;
//
//		char chSet[4] = { '0', '2', '3', '4' };
//		BOOL bCheck = CheckInSet(pTXRInfo->chContent[1], chSet, 4);
//		if (!bCheck)
//			return CUNPACK_NULL;
//
//		char buffer[512];
//
//		int bits = hexchars_bytes((unsigned char*)buffer, sizeof(buffer), (char *)(pTXRInfo->chContent + 2), pTXRInfo->nContentLen - 2);
//		memcpy_s((unsigned char*)pTXRInfo->chContent, sizeof(pTXRInfo->chContent), (unsigned char*)buffer, bits / 8 + (bits % 8 ? 1 : 0));
//		pTXRInfo->nContentLen = bits / 8 + (bits % 8 ? 1 : 0);
//		pTXRInfo->chContent[pTXRInfo->nContentLen] = '\0';
//	}
//	pTXRInfo->nDestionID = m_uLocalID;
//
//	Len = sizeof(TXRInfo);
//	return CUNPACK_TXR;
//}

char* subfield(const char* str, char out[512])
{
	int count = 0;
	const char* pos_end;
	const char* pos_start = strchr(str, ',');
	if (pos_start == NULL) return NULL;

	pos_end = strchr(pos_start + 1, ',');

	if (pos_start&&pos_end)
	{
		int i = 0;
		for (; i < pos_end - pos_start - 1; i++)
			out[count++] = (*(pos_start + 1 + i));//

	}
	else if (pos_end == NULL){
		pos_start++;
		while ((*pos_start) != '\0')
			out[count++] = (*pos_start++);
	}
	out[count] = '\0';

	return (char*)pos_end;
}
void  bdsvr_exp_cb(int msgid, void* msg, int len, void* param)
{
	sess_t sess;
	p2p_t  p2p;
	int  msgcode = 0;
	unsigned char* frame = (unsigned char*)msg;
	bdsvr_exp_t exp = (bdsvr_exp_t)param;
	
	sess   = (sess_t)_BASE(exp).param;
	p2p    = (p2p_t)sess;

	if(exp == NULL || sess == NULL) return;

	switch(msgid)
	{
	case cunpack_txa:
		{
			g_Remoteinstance.SetRemoteAgentStatus(__remote_hasjob);
			char field[512];
			TXAInfo txa = {0};

			char* pstr = (char*)frame;
			pstr = subfield(pstr, field);
			if (pstr == NULL) break;
			txa.nUserId = atoi(field);

			pstr = subfield(pstr, field);
			if (pstr == NULL) break;
			txa.byCommCat = atoi(field);

			pstr = subfield(pstr, field);
			if (pstr == NULL) break;
			txa.byCodeType = atoi(field);

			pstr = subfield(pstr, field);
			char* pos = strrchr(field, '*');
			if (pos == NULL) break;

			*pos = '\0';
			int bits = hexchars_bytes((unsigned char*)txa.chContent, sizeof(txa.chContent), field + 2, strlen(field) - 2);
			txa.nContentLen = bits / 8;
			const CDevice *pFreeDevice = g_Remoteinstance.GetBDArray()->GetFreeDevice();
			if (pFreeDevice)
			{
				g_Remoteinstance.GetBDArray()->AssignWork((CDevice *)pFreeDevice, txa.chContent, txa.nContentLen, txa.nUserId);
			}
			printf("remote has job!\n");
		}
		break;
	case cunpack_ide:
		{
			g_Remoteinstance.SetRemoteAgentStatus(__remote_nojob);
			printf("remote no job!\n");
		}
		break;
	default:
		break;

	}
	
}
