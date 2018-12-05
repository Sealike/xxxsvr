#include "Rdss4_0SearchExp_BONA.h"
#include "GlobalFunction.h"
#include "protocal/Tool.h"

#include <algorithm>
#include <string.h>
#include <vector>
#include <assert.h>

using namespace std;

static SInfoTitle RDSS4_0Title[] =
{
	{CUNPACK_HZR,"$TXHZ"},
	{CUNPACK_SBX,"$BBXX"},
	{CUNPACK_GXM,"$GLXX"},
	{CUNPACK_LZM,"$LZXX"},
	{CUNPACK_GGA,"$GNPX"},
	{CUNPACK_BSI,"$GLZK"},
	{CUNPACK_ZTI,"$ZJXX"},//������Ϣ
	{CUNPACK_DWR,"$DWXX"},//��λ��Ϣ
	{CUNPACK_FKI,"$FKXX"},//������Ϣ
	{CUNPACK_ICI,"$ICXX"},//�ӽ���ģ����Ϣ
	{CUNPACK_TXR,"$TXXX"},//ͨ����Ϣ
	{CUNPACK_ZTI,"$ZJXX"},//�û��豸״̬��Ϣ
	{CUNPACK_ZDA,"$GNTX"}, //UTCʱ�䣬���ڣ��ͱ���ʱ������Ϣ	GJ 2016.3.29
};

static GGAInfo  g_ggaPos;

CRdss4_0SearchExp_BONA::CRdss4_0SearchExp_BONA()
{
	SetRdssTitles();
}

CRdss4_0SearchExp_BONA::~CRdss4_0SearchExp_BONA()
{

}

void CRdss4_0SearchExp_BONA::SetRdssTitles()
{
	m_vRdssTitle.push_back("$LZXX");
	m_vRdssTitle.push_back("$GNPX");//λ����Ϣ
	m_vRdssTitle.push_back("$DWXX");//��λ��Ϣ
	m_vRdssTitle.push_back("$TXXX");//ͨ����Ϣ
	m_vRdssTitle.push_back("$GLZK");//����״��
	m_vRdssTitle.push_back("$ZJXX");//�Լ���Ϣ
	m_vRdssTitle.push_back("$GLXX");//������Ϣ
	m_vRdssTitle.push_back("$ILXX");//I·��Ϣ
	m_vRdssTitle.push_back("$QLXX");//Q·��Ϣ
	m_vRdssTitle.push_back("$ICXX");//IC��Ϣ
	m_vRdssTitle.push_back("$ZHQR");//�Ի�ȷ��
	m_vRdssTitle.push_back("$GNTX");//UTCʱ�䣬���ڣ��ͱ���ʱ������Ϣ  GJ 2016.3.39
	m_vRdssTitle.push_back("$FKXX");//������Ϣ
	m_vRdssTitle.push_back("$BBXX");//�汾��Ϣ
	m_vRdssTitle.push_back("$TXHZ");//��ִ��Ϣ GJ 2016.3.39
	
}

static bool DecodeZDAInfo(BYTE byFrame[],int nFrameLen,lpZDAInfo pZDAInfo)
{
	int iter=10;
	UINT uTemp;
	BYTE byTemp;
	__read_byte(byFrame,iter,byTemp);  //GJ 2016.3.29
	pZDAInfo->nTimeArea=byTemp;
	__read_byte(byFrame,iter,byTemp);   //GJ 2016.3.29
	pZDAInfo->wYear=byTemp+2000;
	__read_byte(byFrame,iter,byTemp);
	pZDAInfo->wMonth=byTemp;
	__read_byte(byFrame,iter,byTemp);
	pZDAInfo->wDay=byTemp;
	__read_byte(byFrame,iter,byTemp);
	pZDAInfo->wHour=byTemp;
	__read_byte(byFrame,iter,byTemp);
	pZDAInfo->wMinute=byTemp;
	__read_byte(byFrame,iter,byTemp);
	pZDAInfo->fSeconds=byTemp;

	return true;
}

static BOOL DecodeGGAInfo(BYTE byFrame[],int nFrameLen,LPGGAInfo pGGAInfo)
{
	int iter = 10;
	UINT  uTemp;
	short sData;
	char  chData;

	__read_byte(byFrame,iter,chData);
	pGGAInfo->chLongEW = chData;

	__read_byte(byFrame,iter,uTemp);
	pGGAInfo->dfLong = uTemp;

	__read_byte(byFrame,iter,uTemp);
	pGGAInfo->dfLong += uTemp/60.0;

	__read_byte(byFrame,iter,uTemp);
	pGGAInfo->dfLong += uTemp/3600.0;

	__read_byte(byFrame,iter,uTemp);
	pGGAInfo->dfLong += uTemp/36000.0;

	__read_byte(byFrame,iter,chData);
	pGGAInfo->chLatNS = chData;

	__read_byte(byFrame,iter,uTemp);
	pGGAInfo->dfLat = uTemp;

	__read_byte(byFrame,iter,uTemp);
	pGGAInfo->dfLat += uTemp/60.0;

	__read_byte(byFrame,iter,uTemp);
	pGGAInfo->dfLat += uTemp/3600.0;

	__read_byte(byFrame,iter,uTemp);
	pGGAInfo->dfLat += uTemp/36000.0;

	__readint_2bytes(byFrame,iter,sData);
	if(sData & 0x8000)//lxl 2016-03-04
		pGGAInfo->dfGndHigh = (sData & 0x7FFF)*-1;
	else
		pGGAInfo->dfGndHigh = (sData & 0x7FFF);

	__readint_2bytes(byFrame,iter,sData);

	__readint_2bytes(byFrame,iter,sData);

	__read_byte(byFrame,iter,chData);
	pGGAInfo->bySatNum = chData;

	__read_byte(byFrame,iter,chData);
	pGGAInfo->byStatus = chData;

	__read_byte(byFrame,iter,chData);
	pGGAInfo->fHdop = chData*0.1;
	pGGAInfo->dfVdop = chData*0.1;

	SYSTEMTIME t;
	GetLocalTime(&t);
	pGGAInfo->nPosHour = t.wHour;
	pGGAInfo->nPosMinute = t.wMinute;
	pGGAInfo->fPosSecond = t.wSecond;

	return TRUE;

}

int CRdss4_0SearchExp_BONA::explain_gga(BYTE byFrame[] , int nFrameLen,BYTE *DataBuff, DWORD &Len)
{
	BOOL bExplain = FALSE;
	BYTE byRtn = CUNPACK_NULL;
	LPGGAInfo pGGA = (LPGGAInfo)DataBuff;
	memset(pGGA,0,sizeof(GGAInfo));
	bExplain = DecodeGGAInfo(byFrame,nFrameLen,pGGA);
	if(bExplain){				
		memcpy(&g_ggaPos,pGGA,sizeof(GGAInfo));
		byRtn = CUNPACK_GGA;
	}
	return byRtn;
}

int CRdss4_0SearchExp_BONA::explain_zda(BYTE byFrame[] , int nFrameLen,BYTE *DataBuff, DWORD &Len)
{
	BOOL bExplain = FALSE;
	BYTE byRtn = CUNPACK_NULL;
	lpZDAInfo pZDAInfo=(lpZDAInfo)DataBuff;
	bExplain=DecodeZDAInfo(byFrame,nFrameLen,pZDAInfo);
	if(bExplain)
	{
		byRtn=CUNPACK_ZDA;
	}
	return byRtn;
}