// Rdss2_1SearchExp.cpp: implementation of the CRdss2_1SearchExp class.
//
//////////////////////////////////////////////////////////////////////
#include "Rdss2_1SearchExp.h"
#include "GlobalFunction.h"
#include <algorithm>
#include "protocal/Tool.h"
//#include <windows.h>

using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//消息常量定义及字头映射定义
///////////////////////////////////////////////////////////////////

static const int RDSSCOUNT = 32;
static SInfoTitle m_titile[] =
{
	{CUNPACK_PKY,"PKY"},
	{CUNPACK_ICM,"ICM"},
	{CUNPACK_PMU,"PMU"},
	{CUNPACK_GSV,"GSV"},
	{CUNPACK_RMC,"RMC"},
	{CUNPACK_GGA,"GGA"},
	{CUNPACK_BSI,"BSI"},//波束信息
	{CUNPACK_DWR,"DWR"},//定位信息
	{CUNPACK_FKI,"FKI"},//反馈信息
	{CUNPACK_ICI,"ICI"},//加解密模块信息
	{CUNPACK_ICZ,"ICZ"},//指挥机下属用户信息
	{CUNPACK_KLS,"KLS"},//口令识别应答
	{CUNPACK_HZR,"HZR"},//回执信息
	{CUNPACK_TXR,"TXR"},//通信信息
	{CUNPACK_SBX,"SBX"},//设备相关参数
	{CUNPACK_ZTI,"ZTI"},//用户设备状态信息
	{CUNPACK_ZDA,"ZDA"}, //UTC时间，日期，和本地时区等信息
	{CUNPACK_LZM,"LZM"},//设备零值信息。
	{CUNPACK_WAA,"WAA"},//位置报告1
	{CUNPACK_WBA,"WBA"},//位置报告2
	{CUNPACK_XSD,"XSD"},//下属定位信息
	{CUNPACK_XST,"XST"},//下属通信信息
	{CUNPACK_GSA,"GSA"},
	{CUNPACK_GXM,"GXM"},
	{CUNPACK_DHV,"DHV"},
	{CUNPACK_ZTG,"VTG"},
	{CUNPACK_RNS,"RNS"},
	{CUNPACK_BID,"BID"},
	{CUNPACK_ZHR,"ZHR"},
	{CUNPACK_APL,"APL"},
	{CUNPACK_GLL,"GLL"},
	{CUNPACK_KLT,"KLT"},
};

////////////////////////////////////////////////////////////////
//构造函数
////////////////////////////////////////////////////////////////
CRdss2_1SearchExp::CRdss2_1SearchExp()
{
	m_dwComBufLen  = 0;
	m_dwCurrPt     = 0;
	m_bSearchStatus= SEARCHMACHINE_NULL;
	m_bCheckSum    = 0;
	m_wFrameIndLen = 0;
	m_uLocalID = 0;

	SetRdssTitles();
	m_nRef++;
	m_ici = NULL;
}

////////////////////////////////////////////////////////////////
//析构函数
////////////////////////////////////////////////////////////////
CRdss2_1SearchExp::~CRdss2_1SearchExp()
{

}

////////////////////////////////////////////////////////////////
//判断是不是协议帧
////////////////////////////////////////////////////////////////
BOOL CRdss2_1SearchExp::IsValidTitles(char* strTitle)
{
	itStr itBgn = m_vRdssTitle.begin();
	itStr itEnd = m_vRdssTitle.end();

	itStr itFind = find(itBgn,itEnd,strTitle);
	if(itFind != itEnd)
		return TRUE;
	else
		return FALSE;
}

////////////////////////////////////////////////////////////////
//初始化协议数组
////////////////////////////////////////////////////////////////
void CRdss2_1SearchExp::SetRdssTitles()
{
	m_vRdssTitle.push_back("$BDGSV");//
	m_vRdssTitle.push_back("$BDRMC");//
	m_vRdssTitle.push_back("$BDGGA");//
	m_vRdssTitle.push_back("$BDDHV");//
	m_vRdssTitle.push_back("$BDGSA");//
	m_vRdssTitle.push_back("$BDZDA"); //UTC时间，日期，和本地时区等信息
	m_vRdssTitle.push_back("$BDBSI");//波束信息
	m_vRdssTitle.push_back("$BDVTG");//波束信息
	m_vRdssTitle.push_back("$BDDWR");//定位信息
	m_vRdssTitle.push_back("$BDFKI");//反馈信息
	m_vRdssTitle.push_back("$BDICI");//加解密模块信息
	m_vRdssTitle.push_back("$BDICZ");//指挥机下属用户信息
	m_vRdssTitle.push_back("$BDKLS");//口令识别应答
	m_vRdssTitle.push_back("$BDHZR");//回执信息
	m_vRdssTitle.push_back("$BDTXR");//通信信息
	m_vRdssTitle.push_back("$BDZTI");//用户设备状态信息
	m_vRdssTitle.push_back("$BDWAA");//位置报告1
	m_vRdssTitle.push_back("$BDXSD");//下属定位信息
	m_vRdssTitle.push_back("$BDXST");//下属通信信息
	m_vRdssTitle.push_back("$BDSBX");//设备相关参数
	m_vRdssTitle.push_back("$BDLZM");//设备零值信息。
	m_vRdssTitle.push_back("$BDGXM");//
	m_vRdssTitle.push_back("$BDPKY");//
	m_vRdssTitle.push_back("$BDICM");//
	m_vRdssTitle.push_back("$BDRNS");//
	m_vRdssTitle.push_back("$BDPMU");//
	m_vRdssTitle.push_back("$BDBID");//
	m_vRdssTitle.push_back("$BDZHR");//
}

////////////////////////////////////////////////////////////////
//检查用户数据是否是合理的
////////////////////////////////////////////////////////////////
BOOL CRdss2_1SearchExp::CheckUserData(const void * pMsg,const UINT uMsgId)
{
	BOOL bOk = FALSE;

	switch(uMsgId)
	{
	case DEVICE_BDECS:
		{//check BDECS
			LPECSStruct  lpECSStruct = (LPECSStruct)pMsg;
			if(lpECSStruct!= NULL)
			{   
				if(strcmp(lpECSStruct->chFreq,"B1") == 0 || strcmp(lpECSStruct->chFreq,"B2") == 0 ||strcmp(lpECSStruct->chFreq,"B3") == 0 || strcmp(lpECSStruct->chFreq,"S") == 0 )
				{
					if(lpECSStruct->byChannelNo >0 && lpECSStruct->byChannelNo < 13)
					{
						if(lpECSStruct->chRoute[0]=='I' || lpECSStruct->chRoute[0]=='Q' || lpECSStruct->chRoute[0]=='A')
						{
							bOk = TRUE;
						}
					}
				}
			}
		}
		break;
	case DEVICE_BDZHS:
		{
			ZHSStruct* pZHSStruct = (ZHSStruct*)pMsg;
			if(pZHSStruct)
				if(pZHSStruct->dwCmd == 0xAA5555AA)
					bOk = TRUE;
		}
		break;
	case DEVICE_BDRIS:
		{
			LPRISStruct pRISStruct = (LPRISStruct)pMsg;
			if(pRISStruct)
				bOk = TRUE;
		}
		break;

	case DEVICE_BDGLS:
		{
			BOOL bCheck = TRUE;
			lpGLSStruct  pGLSStruct = (lpGLSStruct)pMsg;

			if(pGLSStruct)
			{
				bCheck = CheckOpenRange(pGLSStruct->dfLat,0.0,90.0);
				if(bCheck)
				{
					bCheck = FALSE;
					bCheck = CheckOpenRange(pGLSStruct->dfLon,0.0,180.0);
				}

				if(bCheck)
				{//set
					char chSet[2] = {'N','S'};
					bCheck = FALSE;
					bCheck = CheckInSet(pGLSStruct->chLatNS,chSet,2);
				}

				if(bCheck)
				{//set
					char chSet[2] = {'E','W'};//set
					bCheck = FALSE;
					bCheck = CheckInSet(pGLSStruct->chLonEW,chSet,2);
				}

				if(bCheck)
				{//set
					char chSet[2] = {'A','P'};
					bCheck = FALSE;
					bCheck = CheckInSet(pGLSStruct->chPrecisionIndicat,chSet,2);
				}

				if(bCheck)
				{//range
					bCheck = FALSE;
					bCheck = CheckCloseRange(pGLSStruct->byInitCat,(BYTE)0,(BYTE)2);
				}

				if(bCheck)
				{//range
					bCheck = FALSE;
					bCheck = CheckCloseRange(pGLSStruct->wMonth,1,12);
				}

				if(bCheck)
				{//range
					bCheck = FALSE;
					bCheck = CheckCloseRange(pGLSStruct->wDay,1,31);
				}

				if(bCheck)
				{//range
					bCheck = FALSE;
					bCheck = CheckCloseRange(pGLSStruct->wHour,0,23);
				}

				if(bCheck)
				{//range
					bCheck = FALSE;
					bCheck = CheckCloseRange(pGLSStruct->wMinute,0,59);
				}

				if(bCheck)
				{//range
					bCheck = FALSE;
					bCheck = CheckCloseRange(pGLSStruct->wSecond,0,59);
				}

			}else
				bCheck = FALSE;

			bOk = bCheck;
		}
		break;
	case DEVICE_BDLPM :
		{
			LPLPMInfo pLpmInfo = (LPLPMInfo)pMsg;
			BOOL bCheck = FALSE;
			if(pLpmInfo)
			{//range
				bCheck = CheckCloseRange(pLpmInfo->byPowerSavingFlag,0,1);
			}
			bOk = bCheck;
		}
		break;
	case DEVICE_BDMSS:
		{
			//////////////////////////////////////////////////////////////////////////...
			LPMSSInfo pMMSInfo = (LPMSSInfo)pMsg;
			BOOL   bCheck = FALSE;
			//set
			char chSet[2] = {'C','Z'};
            bCheck = CheckInSet(pMMSInfo->chModeCorZ,chSet,2);

			if(bCheck)
			{//set
				bCheck = FALSE;
				if(strcmp((char*)&(pMMSInfo->chFreq[0]),"B1")== 0 || strcmp((char*)&(pMMSInfo->chFreq[0]),"B2")== 0|| strcmp((char*)&(pMMSInfo->chFreq[0]),"B3")== 0|| strcmp((char*)&(pMMSInfo->chFreq[0]),"S")== 0|| strcmp((char*)&(pMMSInfo->chFreq[0]),"L1")== 0)
					bCheck = TRUE;
			}

			if(bCheck)
			{//set
				bCheck = FALSE;
				if(strcmp((char*)&(pMMSInfo->chRoute[0]),"C")== 0 || strcmp((char*)&(pMMSInfo->chRoute[0]),"P")== 0|| strcmp((char*)&(pMMSInfo->chRoute[0]),"A")== 0)
					bCheck = TRUE;
			}

			if(bCheck)
			{//set
				bCheck = FALSE;
				if(strcmp((char*)&(pMMSInfo->chSys[0]),"BD")== 0 || strcmp((char*)&(pMMSInfo->chSys[0]),"GP")== 0|| strcmp((char*)&(pMMSInfo->chSys[0]),"GL")== 0)
					bCheck = TRUE;
			}

			bOk = bCheck;
		}
		break;
	case DEVICE_BDBSX:
		{
			LPBSXInfo pBSXInfo = (LPBSXInfo)pMsg;
			if(pBSXInfo)
				bOk = TRUE;
		}
		break;
	case DEVICE_BDBSQ:
		{
			LPBSQInfo pBSQInfo = (LPBSQInfo)pMsg;
			if(pBSQInfo)
				bOk = TRUE;
		}
		break;
	case DEVICE_BDBSD:
		{
           LPBSDInfo pBSDInfo = (LPBSDInfo)pMsg;
		   if(pBSDInfo)
				bOk = TRUE;
		}
		break;
	case DEVICE_BDBSS:
		{
			pBSSZ_STRUCT f_Buff_BSSZ;
			f_Buff_BSSZ = (pBSSZ_STRUCT)pMsg;
			
			BOOL bCheck = FALSE;
			if(f_Buff_BSSZ)
				bCheck = TRUE;
			
			if(bCheck)
			{//range
				bCheck = FALSE;
				bCheck = CheckCloseRange(f_Buff_BSSZ->beamDiff,0,10);
			}
			
			if(bCheck)
			{//range
				bCheck = FALSE;
				bCheck = CheckCloseRange(f_Buff_BSSZ->beamNo,0,10);
			}
			//range
			if(f_Buff_BSSZ->beamDiff == f_Buff_BSSZ->beamNo)
				bCheck = FALSE;
			
			bOk = bCheck;
		}

		break;
	case DEVICE_BSCX:
		{//波束查询
			pBSCX_STRUCT f_BSCX;
			f_BSCX = (pBSCX_STRUCT)pMsg;
			if(f_BSCX)
			{
				bOk = TRUE;
			}else
				bOk = FALSE;
		}
		break;
	case DEVICE_BDKLS:
		{
			BOOL bCheck = FALSE;
			lpKLSInfo pKLSB = (lpKLSInfo)pMsg;

			if(pKLSB)
			{   //set
				bCheck = FALSE;
				char chSet[2] = {'Y','N'};
				bCheck = CheckInSet(pKLSB->chResponseFlag,chSet,2);
			}

			bOk = bCheck;
		}
		break;
	case DEVICE_BDLZM:
		{
			LPLZMInfo pLZMInfo = (LPLZMInfo)pMsg;
			if(pLZMInfo)
				bOk = (pLZMInfo->byManageMode == 1 || pLZMInfo->byManageMode == 2);
			else
				bOk = FALSE;
		}
		break;
	case DEVICE_BDKLT:
		{

			LPKLTStruct pKLSB = (LPKLTStruct)pMsg;
			
			BOOL bCheck = FALSE;
			char chSet[2] = {'P','Z'};
			
			if(pKLSB)
			{   //set
				bCheck = CheckInSet(pKLSB->chCharacter,chSet,2);
			}
			
			if(bCheck)
			{//range
				bCheck = FALSE;
				bCheck = CheckCloseRange(pKLSB->byCommType,0,2);
			}
			
			bOk = bCheck;

		}
		break;
	case DEVICE_BDJMS:
		{//静默设置
			pBDJMS_STRUCT pBDJMS = (pBDJMS_STRUCT)pMsg;
			if(pBDJMS)
			{
				if(pBDJMS->bInhabit >=0 && pBDJMS->bInhabit <=2)//
				{
					bOk = TRUE;
				}else
					bOk = FALSE;
			}else
				bOk = FALSE;
		}
		break;
		//14 RDSS 检测设备。
	case DEVICE_BDICA:
		{
			pBDICA_STRUCT pInfo  = (pBDICA_STRUCT )pMsg;

			if(pInfo->byType==0 || pInfo->byType == 1)
				bOk = TRUE;
			else
				bOk = FALSE;
		}
		break;
	case DEVICE_BDWAA:
		{
			BOOL bCheck = FALSE;
			lpWAAInfo pWAAInfo  = (lpWAAInfo)pMsg;
			if(pWAAInfo->byInfoType == 1)
				bCheck = TRUE;
			
			if(bCheck)
			{//range
				bCheck = FALSE;
				bCheck = CheckCloseRange(pWAAInfo->wReportHour,0,23);
			}
			
			if(bCheck)
			{//range
				bCheck = FALSE;
				bCheck = CheckCloseRange(pWAAInfo->wReportMinute,0,59);
			}
			
			if(bCheck)
			{//range
				bCheck = FALSE;
				bCheck = CheckLCloseRange(pWAAInfo->fReportSecond,0,60);
			}
			
			if(bCheck)
			{//range
				bCheck = FALSE;
				bCheck = CheckCloseRange(pWAAInfo->dfLong,0,180);
			}
			
			if(bCheck)
			{//range
				bCheck = FALSE;
				bCheck = CheckCloseRange(pWAAInfo->dfLat,0,90);
			}
			
			if(bCheck)
			{//set
				bCheck = FALSE;
				char chSet[2] = {'N','S'};
				bCheck = CheckInSet(pWAAInfo->chLatDir,chSet,2);
			}
			
			if(bCheck)
			{//set
				bCheck = FALSE;
				char chSet[2] = {'E','W'};
				bCheck = CheckInSet(pWAAInfo->chLongDir,chSet,2);
			}
			
			bOk = bCheck;
		}
		break;

	case DEVICE_BDWBA:
		{
			BOOL bCheck = FALSE;

			lpWBAInfo pWBAInfo  = (lpWBAInfo)pMsg;
			if(pWBAInfo)
				bCheck = TRUE;

			if(bCheck)
			{
				bCheck = FALSE;
				if(pWBAInfo->chHgtType == 'H' || pWBAInfo->chHgtType == 'L')
					bCheck = TRUE;
			}

			if(bCheck)
			{
				bCheck = FALSE;
				if(pWBAInfo->fReportFreq>=0)
					bCheck = TRUE;
			}

			bOk = bCheck;
		}
		break;
	case DEVICE_WZBG:
		{
			lpWZBGgx pWZBGgx = (lpWZBGgx)pMsg;

			BOOL bCheck = FALSE;

			if(pWZBGgx)
				bCheck = TRUE;
			//range
			if(bCheck)
			{
				bCheck = FALSE;
				bCheck = CheckCloseRange(pWZBGgx->T.wMonth,1,12);
			}
			//range
			if(bCheck)
			{
				bCheck = FALSE;
				bCheck = CheckCloseRange(pWZBGgx->T.wDay,1,31);
			}
			//range
			if(bCheck)
			{
				bCheck = FALSE;
				bCheck = CheckCloseRange(pWZBGgx->T.wHour,1,23);
			}
			//range
			if(bCheck)
			{
				bCheck = FALSE;
				bCheck = CheckCloseRange(pWZBGgx->T.wMinute,1,59);
			}
			//range
			if(bCheck)
			{
				bCheck = FALSE;
				bCheck = CheckCloseRange(pWZBGgx->T.wSecond,1,59);
			}
			//range
			if(bCheck)
			{
				bCheck = FALSE;
				if(pWZBGgx->byType == 0 || pWZBGgx->byType == 1)
					bCheck = TRUE;
			}
			//range
			if(bCheck)
			{
				bCheck = FALSE;
				bCheck = CheckOpenRange(pWZBGgx->dfLat,-90,90);
			}
			//range
			if(bCheck)
			{
				bCheck = FALSE;
				bCheck = CheckOpenRange(pWZBGgx->dfLon,-180,180);
			}

			bOk = bCheck;
		}
		break;
	case DEVICE_BDTXA:
		{
			lpTXAInfo pTXAInfo = (lpTXAInfo)pMsg;
			BOOL bCheck = FALSE;
			
			if(pTXAInfo)
				bCheck = TRUE;
			//range
			if(bCheck)
			{
				bCheck = FALSE;
				if(pTXAInfo->byCommCat == 0 || pTXAInfo->byCommCat == 1)
					bCheck = TRUE;
			}
			//range
			if(bCheck)
			{
				bCheck = FALSE;
				bCheck = CheckCloseRange(pTXAInfo->byCodeType,0,2);
			}
			//range
			if(bCheck)
			{
				bCheck = FALSE;
				if(pTXAInfo->nContentLen)
					bCheck = TRUE;
			}
			
			bOk = bCheck;
		}
		break;

	case DEVICE_BDGXM:
		{//管理信息
			lpGXMInfo pGXMInfo = (lpGXMInfo)pMsg;
			BOOL bCheck = FALSE;

			if(pGXMInfo)
				bCheck  = TRUE;
			//range
			if(bCheck)
			{
				bCheck  = FALSE;
				if(pGXMInfo->nType == 1 || pGXMInfo->nType ==2)
					bCheck = TRUE;
			}

			bOk = bCheck;
		}
		break;
		//6.RDSS定位申请配置指令
	case DEVICE_BDDWA:
		{
			pBDDWA_STRUCT f_pBuff_DWSQ;
			f_pBuff_DWSQ = (pBDDWA_STRUCT)pMsg;
			BOOL bCheck = FALSE;

			if(f_pBuff_DWSQ)
				bCheck = TRUE;
			//range
			if(bCheck)
			{
				bCheck = FALSE;
				bCheck = CheckCloseRange(f_pBuff_DWSQ->bPosType,0,3);
			}

			bOk = bCheck;
		}
		break;
		//7.RDSS定时申请配置指令
	case DEVICE_BDDSA:
		{//定时申请配置指令
			UINT i = 0;
			pDSSQ_STRUCT f_pBuff_DSSQ;
			f_pBuff_DSSQ = (pDSSQ_STRUCT)pMsg;
			BOOL bCheck = FALSE;

			if(f_pBuff_DSSQ)
				bCheck = TRUE;
			//range
			if(bCheck)
			{
				bCheck = FALSE;
				if(f_pBuff_DSSQ->nType == 1 || f_pBuff_DSSQ->nType == 2)
				{
					bCheck = TRUE;
				}
			}

			if(bCheck)
			{
				bCheck = FALSE;
				if(f_pBuff_DSSQ->userAddr)
					bCheck = TRUE;
			}

			if(bCheck)
			{
				bCheck = FALSE;
				bCheck = CheckOpenRange(f_pBuff_DSSQ->fB,-90,90);
			}

			if(bCheck)
			{
				bCheck = FALSE;
				bCheck = CheckOpenRange(f_pBuff_DSSQ->fL,-180,180);
			}

			bOk = bCheck;
		}
		break;
		//////////////
		//8.RDSS通信查询指令
		//////////////
	case DEVICE_BDCXA:
		{
			lpBDCXA_STRUCT pBDCXA_STRUCT;
			pBDCXA_STRUCT = (lpBDCXA_STRUCT)pMsg;
			BOOL bCheck = FALSE;
			if(pBDCXA_STRUCT)
				bCheck = TRUE;

			if(bCheck)
			{//range
				bCheck = FALSE;
				if(pBDCXA_STRUCT->byQueryCat == 0 || pBDCXA_STRUCT->byQueryCat == 1)
					bCheck = TRUE;
			}
			if(bCheck)
			{//range
				bCheck = TRUE;
				CheckCloseRange(pBDCXA_STRUCT->byQueryType,1,3);
			}

			bOk = bCheck;
		}
		break;

	case DEVICE_BDPKY:
		{
			LPPKYInfo pPKYInfo = (LPPKYInfo)pMsg;
			bOk = TRUE;
		}
		break;
	case DEVICE_BDRMO:
		{
			lpRMOInfo pRMOInfo = (lpRMOInfo)pMsg;
			if(pRMOInfo)
				bOk = CheckCloseRange(pRMOInfo->byMode,1,4);
			else
				bOk = FALSE;
		}
		break;
	case DEVICE_SATDEL:
		{
			LPSatDelete pSatDelete = (LPSatDelete)pMsg;
			BOOL bCheck = FALSE;
			
			if(pSatDelete)
				bCheck = TRUE;
			//range
			if(bCheck)
			{
				bCheck = FALSE;
				bCheck = CheckCloseRange(pSatDelete->nSys,0,3);
			}
			//range
			if(bCheck)
			{
				bCheck = FALSE;
				bCheck = CheckCloseRange(pSatDelete->nFreq,0,3);
			}
			//range
			if(bCheck)
			{
				bCheck = FALSE;
				bCheck = CheckCloseRange(pSatDelete->byOper,0,1);
			}
			
			bOk = bCheck;
		}
		break;
	case DEVICE_MODPPS:
		{
			LPOnePPSCorr pCorr = (LPOnePPSCorr)pMsg;
			if(pCorr)
			{//range
				bOk = CheckCloseRange(pCorr->nSys,0,3);
			}
			else
			{
				bOk = FALSE;
			}
		}
		break;
	case  DEVICE_BDRNS:
		{
			LPRNSCmd pRNSCmd = (LPRNSCmd)pMsg;
			if(pRNSCmd)
				bOk = TRUE;
		}
		break;

	case DEVICE_BDAPL:
		{
			tagAPLCmd *pAPLCmd = (tagAPLCmd *)(pMsg);
			BOOL  bCheck = FALSE;

			if(pAPLCmd)
				bCheck = TRUE;
			//range
			if(bCheck)
			{
				bCheck = FALSE;
				bCheck = CheckCloseRange(pAPLCmd->nHour,1,23);
			}
			//range
			if(bCheck)
			{
				bCheck = FALSE;
				bCheck = CheckCloseRange(pAPLCmd->nMinute,1,59);
			}
			//range
			if(bCheck)
			{
				bCheck = FALSE;
				bCheck = CheckCloseRange(pAPLCmd->fSecond,0.0,60.0);
			}

			bOk = bCheck;
		}
		break;
	default :
		break;
	}

	return bOk;
}

//////////////////////////////////////////////////////////////////////////
///
///encode ..
/////////////////////////////////////////////////////////////////////////////
int  CRdss2_1SearchExp::EncodeFrame(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen)
{
	DWORD f_EncodeDataLen=0;
	if(pMsg == NULL || nMsgBlockLen == 0 || byFrameBuff == NULL || nFrameBuffLen ==0)
		return 0;

	BOOL bCheck = CheckUserData(pMsg,uMsgId);
	if(!bCheck)
	{
		TRACE1("check user data failed! %d \r\n",uMsgId);
		return 0;
	}

	switch(uMsgId)
	{
	case DEVICE_BDECS://$CCRMO,DBG,2,1.0*00\r\n;ect;
		{   //hougaogong
			LPECSStruct  lpECSStruct = (LPECSStruct)pMsg;
			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCECS,%s,%02d,%C",lpECSStruct->chFreq,lpECSStruct->byChannelNo,lpECSStruct->chRoute);
		}
		break;
	case DEVICE_BDZHS:
		{//modf liu
			LPZHSStruct pZHSStruct = (LPZHSStruct)pMsg;
			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCZHS,%08X",pZHSStruct->dwCmd);
		}
		break;
	case DEVICE_BDRIS:
		{//modf liu
			LPRISStruct pRISStruct = (LPRISStruct)pMsg;
			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCRIS,");
		}
		break;

	case DEVICE_BDGLS:
		{   //modf liu
			f_EncodeDataLen = 0;
			lpGLSStruct  pGLSStruct = (lpGLSStruct)pMsg;//modf liu
			f_EncodeDataLen += sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCGLS,%s,%C,%s,%C,%f,%C,%C,%02d02d02d,02d02d02d,%d",\
				CSTRLATFIELD(pGLSStruct->dfLat),pGLSStruct->chLatNS,CSTRLONFIELD(pGLSStruct->dfLon),pGLSStruct->chLonEW,pGLSStruct->fHgt,\
				pGLSStruct->chHgtUnit,pGLSStruct->chPrecisionIndicat,(pGLSStruct->nYear)%100,pGLSStruct->wMonth,pGLSStruct->wDay,\
				pGLSStruct->wHour,pGLSStruct->wMinute,pGLSStruct->wSecond,pGLSStruct->byInitCat);
		}
		break;
	case DEVICE_BDLPM :
		{//modf liu
			LPLPMInfo pLpmInfo = (LPLPMInfo)pMsg;
			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCLPM,%d",pLpmInfo->byPowerSavingFlag);
		}
		break;
	case DEVICE_BDMSS:
		{
			//////////////////////////////////////////////////////////////////////////...
			LPMSSInfo pMMSInfo = (LPMSSInfo)pMsg;
			f_EncodeDataLen  = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCMSS,%C,%d,%s,%s,%s,%s,%s,%s",pMMSInfo->chModeCorZ,\
				pMMSInfo->byTypeItem,pMMSInfo->chFreq[0],pMMSInfo->chRoute[0],pMMSInfo->chFreq[1],pMMSInfo->chRoute[1],pMMSInfo->chFreq[2],pMMSInfo->chRoute[2]);

		}
		break;
	case DEVICE_BDBSX:
		{//modf liu
			LPBSXInfo pBSQInfo = (LPBSXInfo)pMsg;
			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCBSX,%s,%d,%d,%d,",pBSQInfo->chName,pBSQInfo->wTotalSentence,pBSQInfo->wCurrSentenceNo,\
				pBSQInfo->wPointNum);

			pBSQInfo->wPointNum = MAX2(pBSQInfo->wPointNum,sizeof(pBSQInfo->posInfo)/sizeof(PositionInfo));
			//modf liu
			for(int i = 0 ; i < pBSQInfo->wPointNum  && i < sizeof(pBSQInfo->posInfo); i++)
			{
				f_EncodeDataLen += sprintf_s((char*)byFrameBuff + f_EncodeDataLen,nFrameBuffLen-f_EncodeDataLen,"%07.2f,%C,%08.2f,%C,%.2f,%C,",\
					pBSQInfo->posInfo[i].dfLat,pBSQInfo->posInfo[i].chLatNS,pBSQInfo->posInfo[i].dfLong,pBSQInfo->posInfo[i].chLongEW,pBSQInfo->posInfo[i].dfHgt,pBSQInfo->posInfo[i].chHgtUnit);
			}
			//消除最后一个逗号。
			f_EncodeDataLen -=1;
		}
		break;
	case DEVICE_BDBSQ:
		{//modf liu
			LPBSQInfo pBSQInfo = (LPBSQInfo)pMsg;
			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCBSQ,%s,%07.2f,%C,%08.2f,%C,%.2f,%C,%f,%C",
				pBSQInfo->chName,pBSQInfo->dfCenterLat,pBSQInfo->chLatNS,pBSQInfo->dfCenterLong,pBSQInfo->chLongEW,pBSQInfo->dfCenterHgt,pBSQInfo->chHgtUnit,pBSQInfo->dfRadius,pBSQInfo->chRadiusUnit);
		}
		break;
	case DEVICE_BDBSD:
		{//modf liu
           LPBSDInfo pBSDInfo = (LPBSDInfo)pMsg;//消除最后一个逗号。
		   f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCBSD,%s,%s,%07.2f,%C,%08.2f,%C,%.2f,%C",
			   pBSDInfo->chName,pBSDInfo->chMapName,pBSDInfo->dfLat,pBSDInfo->chLatNS,pBSDInfo->dfLong,pBSDInfo->chLongEW,pBSDInfo->dfHgt,pBSDInfo->chHgtUnit);
		}
		break;
	case DEVICE_BDBSS:
		{
			pBSSZ_STRUCT f_Buff_BSSZ;
			f_Buff_BSSZ = (pBSSZ_STRUCT)pMsg;
			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCBSS,%02d,%02d",
				f_Buff_BSSZ->beamNo,f_Buff_BSSZ->beamDiff);
		}

		break;
	case DEVICE_BSCX:
		{//modf liu
			pBSCX_STRUCT f_BSCX;
			f_BSCX = (pBSCX_STRUCT)pMsg;
			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$BSCX,%d",f_BSCX->userAddr);
		}
		break;
	case DEVICE_BDKLS:
		{
			lpKLSInfo pKLSB = (lpKLSInfo)pMsg;

			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCKLS,%07d,%C",pKLSB->nSubUsrId ,pKLSB->chResponseFlag);
		}
		break;
	case DEVICE_BDLZM:
		{
			LPLZMInfo pLZMInfo = (LPLZMInfo)pMsg;
			if(pLZMInfo->byManageMode != 1)
				f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCLZM,%d,%d",pLZMInfo->byManageMode,(int)pLZMInfo->dfZeroValue);
			else
				f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCLZM,%d,",pLZMInfo->byManageMode);			
		}
		break;
	case DEVICE_BDKLT:
		{
			LPKLTStruct pKLSB = (LPKLTStruct)pMsg;
			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCKLT,%C,%07d,%d,%s",pKLSB->chCharacter,pKLSB->nUsrId,pKLSB->byCommType,pKLSB->chContent);
		}
		break;
	case DEVICE_BDJMS:
		{//静默设置
			pBDJMS_STRUCT pBDJMS = (pBDJMS_STRUCT)pMsg;
			char chFlag;
			//静默设置
			if(pBDJMS->bInhabit == 0)
				chFlag = 'N';
			else  if(pBDJMS->bInhabit == 1)
			{
				chFlag = 'E';
			}else if(pBDJMS->bInhabit == 2)
			{
				chFlag = 'Z';
			}
			//modf liu
			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCJMS,%C",chFlag);
		}
		break;
		//14 RDSS 检测设备。
	case DEVICE_BDICA:
		{
			pBDICA_STRUCT pInfo  = (pBDICA_STRUCT )pMsg;
			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCICA,%d,%02d",pInfo->byType,pInfo->nSubUsrFrameNo);
		}
		break;
	case DEVICE_BDWAA:
		{
			char chBuffer[128] = {0};
			//
			lpWAAInfo pWAAInfo  = (lpWAAInfo)pMsg;
			//
		
			sprintf_s(chBuffer,128,"%02d%02d%05.2f,",pWAAInfo->wReportHour,pWAAInfo->wReportMinute,pWAAInfo->fReportSecond);
			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCWAA,%d,%d,%07d,",pWAAInfo->byInfoType,pWAAInfo->nReportFreq,pWAAInfo->nUserId);
			f_EncodeDataLen += sprintf_s((char*)byFrameBuff+f_EncodeDataLen,nFrameBuffLen-f_EncodeDataLen,"%s",chBuffer);
			//
			sprintf_s(chBuffer,128,"%7.6f,",((int)pWAAInfo->dfLat)*100.0 + (pWAAInfo->dfLat - (int)pWAAInfo->dfLat)*60 );
			//
			f_EncodeDataLen += sprintf_s((char*)byFrameBuff+f_EncodeDataLen,nFrameBuffLen-f_EncodeDataLen,"%s",chBuffer);
			f_EncodeDataLen += sprintf_s((char*)byFrameBuff+f_EncodeDataLen,nFrameBuffLen-f_EncodeDataLen,"%C,",pWAAInfo->chLatDir);
			///位置
			sprintf_s(chBuffer,128,"%8.6f,",((int)pWAAInfo->dfLong)*100.0 + (pWAAInfo->dfLong - (int)pWAAInfo->dfLong)*60);
			f_EncodeDataLen += sprintf_s((char*)byFrameBuff+f_EncodeDataLen,nFrameBuffLen-f_EncodeDataLen,"%s",chBuffer);
			f_EncodeDataLen += sprintf_s((char*)byFrameBuff+f_EncodeDataLen,nFrameBuffLen-f_EncodeDataLen,"%C,",pWAAInfo->chLongDir);
			f_EncodeDataLen += sprintf_s((char*)byFrameBuff+f_EncodeDataLen,nFrameBuffLen-f_EncodeDataLen,"%.1f,",pWAAInfo->dfHgt);
			f_EncodeDataLen += sprintf_s((char*)byFrameBuff+f_EncodeDataLen,nFrameBuffLen-f_EncodeDataLen,"%C",pWAAInfo->chHgtUnit);
		}
		break;
		//位置报告 
	case DEVICE_BDWBA:
		{//modf liu
			lpWBAInfo pWBAInfo  = (lpWBAInfo)pMsg;
			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCWBA,%07d,%C,%f,%.2f",pWBAInfo->nUserId,pWBAInfo->chHgtType,\
				pWBAInfo->dfAnteHgt,pWBAInfo->fReportFreq);

		}
		break;
	case DEVICE_WZBG:
		{///
			lpWZBGgx pWZBGgx = (lpWZBGgx)pMsg;
			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$WZBG,%d,%d,%d,%d,%d,%02d%02d%02d,%02d%02d%02d,%.7f,%.7f,%.7f,%.7f",
				pWZBGgx->uLocalId,pWZBGgx->bOn,pWZBGgx->uRcvId,pWZBGgx->uFreq,
				pWZBGgx->byType,pWZBGgx->T.wYear,pWZBGgx->T.wMonth,pWZBGgx->T.wDay,
				pWZBGgx->T.wHour,pWZBGgx->T.wMinute,pWZBGgx->T.wSecond,
				pWZBGgx->dfLon,pWZBGgx->dfLat,pWZBGgx->dfHgt,pWZBGgx->dfAnteHgt
				);
		} 
		break;
	case DEVICE_BDTXA:
		{
			lpTXAInfo pTXAInfo = (lpTXAInfo)pMsg;
			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCTXA,%07d,%d,%d,",pTXAInfo->nUserId,pTXAInfo->byCommCat,\
				pTXAInfo->byCodeType);

			if((pTXAInfo->nContentLen + f_EncodeDataLen) > nFrameBuffLen)
				return -1;

			CToolClass<lpICIInfo> tool(m_ici);
			int commbits = tool.GetCommBitsLen(pTXAInfo->byCommCat==0);
			if(commbits <=0) return -1;

			if(pTXAInfo->byCodeType == 2)//2016-03-04 lxl
			{
				char buffer[512];
				byFrameBuff[f_EncodeDataLen++] = 'A';
				byFrameBuff[f_EncodeDataLen++] = '4';
				int contentbits = pTXAInfo->nContentLen*8;
				if(contentbits>commbits-8){
					contentbits = commbits-8;
				}

				int countbytes = bytes_hexchars(buffer,sizeof(buffer),(BYTE*)pTXAInfo->chContent,contentbits);
				memcpy(byFrameBuff+f_EncodeDataLen,buffer,countbytes);
				f_EncodeDataLen+=countbytes;
			}else if(pTXAInfo->byCodeType == 1){
				int contentbits = pTXAInfo->nContentLen*4;
				if(contentbits>commbits)
					contentbits = commbits;
				
				int countbytes = contentbits/4;

				memcpy(byFrameBuff+f_EncodeDataLen,pTXAInfo->chContent,countbytes);
				f_EncodeDataLen += countbytes;
			}else if(pTXAInfo->byCodeType == 0){
				int contentbits = pTXAInfo->nContentLen/2*14;
				if(contentbits>commbits){
					contentbits = commbits;
				}
				int countbytes = contentbits/14*2;
				memcpy(byFrameBuff+f_EncodeDataLen,pTXAInfo->chContent,countbytes);
				f_EncodeDataLen += countbytes;
			}else
				return -1;
			
		}
		break;

	case DEVICE_BDGXM:
		{
			lpGXMInfo pGXMInfo = (lpGXMInfo)pMsg;
			if(pGXMInfo->nType != 2)
				f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCGXM,%d,%s,%d",pGXMInfo->nType,pGXMInfo->chMagInfo,pGXMInfo->nSerialNo);
			else
				f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCGXM,%d,",pGXMInfo->nType);
		}
		break;
		//6.RDSS定位申请配置指令
	case DEVICE_BDDWA:
		pBDDWA_STRUCT f_pBuff_DWSQ;//
		f_pBuff_DWSQ = (pBDDWA_STRUCT)pMsg;//modf liu
		f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCDWA,%07d,%C,%d,%C,%f,%f,%f,%f,%03d",
			f_pBuff_DWSQ->userAddr,	f_pBuff_DWSQ->bifUrgency ? 'A':'V',
			f_pBuff_DWSQ->bPosType,
			f_pBuff_DWSQ->bifHigh ? 'H':'L',
			f_pBuff_DWSQ->dfHgt,
			f_pBuff_DWSQ->fAntennaHgt,
			f_pBuff_DWSQ->fAirPressure,
			f_pBuff_DWSQ->fTemperature,
			f_pBuff_DWSQ->wFreq
			);
		break;
		//7.RDSS定时申请配置指令
	case DEVICE_BDDSA:
		{   //
			UINT i = 0;
			pDSSQ_STRUCT f_pBuff_DSSQ;
			f_pBuff_DSSQ = (pDSSQ_STRUCT)pMsg;
			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCDSA,%07d,%d,%C,",
				f_pBuff_DSSQ->userAddr,\
				f_pBuff_DSSQ->nType,\
				f_pBuff_DSSQ->bifHavePos ? 'A':'V'
				);

			double dfData;
			dfData = ((int)(f_pBuff_DSSQ->fB))*100+(f_pBuff_DSSQ->fB -(int)f_pBuff_DSSQ->fB)*60;
			char strData[128];
			sprintf_s(strData,128,"%11.6f",dfData);

			for(i = 0 ; i < strlen(strData)&& i < sizeof(strData); i++)
			{
				if(strData[i] == ' ')
				{
					strData[i] ='0';
				}
			}
			f_EncodeDataLen += sprintf_s((char*)byFrameBuff+f_EncodeDataLen,nFrameBuffLen-f_EncodeDataLen,"%s,",strData);

			dfData = ((int)(f_pBuff_DSSQ->fL))*100+(f_pBuff_DSSQ->fL -(int)f_pBuff_DSSQ->fL)*60;
			sprintf_s(strData,128,"%12.6f",dfData);
			for(i = 0 ; i < strlen(strData) && i < sizeof(strData); i++)
			{
				if(strData[i] == ' ')
				{
					strData[i] ='0';
				}
			}
			f_EncodeDataLen += sprintf_s((char*)byFrameBuff+f_EncodeDataLen,nFrameBuffLen-f_EncodeDataLen,"%s,",strData);			
			f_EncodeDataLen += sprintf_s((char*)byFrameBuff+f_EncodeDataLen,nFrameBuffLen-f_EncodeDataLen,"%.1f,",f_pBuff_DSSQ->dwFreq*1.0);
			f_EncodeDataLen += sprintf_s((char*)byFrameBuff+f_EncodeDataLen,nFrameBuffLen-f_EncodeDataLen,"%.1f,",f_pBuff_DSSQ->iUniZero*1.0);
			f_EncodeDataLen += sprintf_s((char*)byFrameBuff+f_EncodeDataLen,nFrameBuffLen-f_EncodeDataLen,"%.1f",f_pBuff_DSSQ->iAddZero*1.0);
		}
		break;
		//8.RDSS通信查询指令
	case DEVICE_BDCXA:
		{
			lpBDCXA_STRUCT pBDCXA_STRUCT;
			pBDCXA_STRUCT = (lpBDCXA_STRUCT)pMsg;
			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCCXA,%d,%d,%07d",
				pBDCXA_STRUCT->byQueryCat,pBDCXA_STRUCT->byQueryType,pBDCXA_STRUCT->nUserId
				);
		}
		break;
		//PKY
	case DEVICE_BDPKY:
		{   
			LPPKYInfo pPKYInfo = (LPPKYInfo)pMsg;
			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCPKY,%d,",pPKYInfo->uWeek);
			memcpy(byFrameBuff+f_EncodeDataLen,pPKYInfo->chSecretKey,6);
			f_EncodeDataLen+= 6;			
			byFrameBuff[f_EncodeDataLen++] = ',';
			byFrameBuff[f_EncodeDataLen++] = ',';
			byFrameBuff[f_EncodeDataLen++] = ',';
			byFrameBuff[f_EncodeDataLen++] = ',';
			byFrameBuff[f_EncodeDataLen++] = ',';
		}
		break;
	case DEVICE_BDRMO:
		{
			 f_EncodeDataLen=encode_rmo(pMsg,uMsgId,nMsgBlockLen, byFrameBuff,nFrameBuffLen);
		}
		break;
	case DEVICE_SATDEL:
		{
			LPSatDelete pSatDelete = (LPSatDelete)pMsg;
			char chSys[4];
			char chFreq[4];
			if(pSatDelete->nSys == __BD)
			{
				sprintf_s(chSys,4,"BD");
			}else if(pSatDelete->nSys == __GPS)
			{
				sprintf_s(chSys,4,"GP");
			}else if(pSatDelete->nSys == __GLO)
				sprintf_s(chSys,4,"GL");
			else if(pSatDelete->nSys ==  __GAL)
				sprintf_s(chSys,4,"GA");			

			if(pSatDelete->nFreq == __B1)
				sprintf_s(chFreq,4,"B1");
			else if(pSatDelete->nFreq == __B2)
				sprintf_s(chFreq,4,"B2");
			else if(pSatDelete->nFreq == __B3)
				sprintf_s(chFreq,4,"B3");
			else if(pSatDelete->nFreq == __L1)
				sprintf_s(chFreq,4,"L1");

			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$SVDEL,%03d,%s,%s,%d",
				pSatDelete->nSatNo,chFreq,chSys,pSatDelete->byOper);
		}
		break;
	case DEVICE_MODPPS:
		{
			char chSys[4];
			LPOnePPSCorr pCorr = (LPOnePPSCorr)pMsg;
			if(pCorr->nSys == __BD)
			{
				sprintf_s(chSys,4,"BD");
			}else if(pCorr->nSys == __GPS)
			{
				sprintf_s(chSys,4,"GP");
			}else if(pCorr->nSys == __GLO)
				sprintf_s(chSys,4,"GL");
			else if(pCorr->nSys ==  __GAL)
				sprintf_s(chSys,4,"GA");

			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$PPSAJ,%s,%.9f",
				chSys,pCorr->dfValue);
		}
		break;
	case  DEVICE_BDRNS:
		{
			LPRNSCmd pRNSCmd = (LPRNSCmd)pMsg;
			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCRNS,%05d,%05d,%05d,%s",pRNSCmd->nTotalPack,pRNSCmd->nCurPack,pRNSCmd->nPackLen,pRNSCmd->chContent);
		}
		break;

	case DEVICE_BDAPL:
		{			
			tagAPLCmd *pAPLCmd = (tagAPLCmd *)(pMsg);
			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCAPL,%02d%02d%04.2f,%f,%c,%f,%c,%f,%c",\
				pAPLCmd->nHour,pAPLCmd->nMinute,(pAPLCmd->fSecond),pAPLCmd->LevelH,'M',pAPLCmd->LevelV,'M',pAPLCmd->Level3D,'M');
		}
		break;

	case DEVICE_SECPULSE:
		{
			LPPulseMode pPulseMode = (LPPulseMode)(pMsg);
			f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCSET,BCODE_1PPS,%d",pPulseMode->nMode);
		}
		break;
	default :
		break;
	}   

	 if(f_EncodeDataLen >0 )
	{
		f_EncodeDataLen += sprintf_s((char*)(byFrameBuff+f_EncodeDataLen),nFrameBuffLen-f_EncodeDataLen,"*%02X\r\n",ComputeCheckSumRdss21((BYTE*)byFrameBuff+1,f_EncodeDataLen-1));
	}
	//设置全局变量	

	return f_EncodeDataLen;
}

//////////////////////////////////////////////////////////////////////////
///
///Process 数据处理
////////////////////////////////////////////////////////////////////////////////
int CRdss2_1SearchExp::ProcessData(const BYTE byBuffData[],const int nDataLen)
{
	BYTE  f_bSearchRes  = 0,f_bInfoID = 0;
	DWORD f_dwExpMsgLen = 0;
	int   nProcessLen   = 0;
    int   nThisLen      = 0;
	DWORD msglen = 0;
	try{
		NULLReturn(byBuffData , 0);
		FALSEReturn(nDataLen>0, 0);

		while(nProcessLen < nDataLen)
		{
			if(sizeof(m_ComBuff)-1  <= nDataLen - nProcessLen)
				nThisLen = sizeof(m_ComBuff)-1;
			else
				nThisLen = nDataLen - nProcessLen;

			m_dwComBufLen = nThisLen  ;
			memcpy(m_ComBuff,byBuffData+nProcessLen,nThisLen);
			m_ComBuff[nThisLen] = '\0';

			do{
				f_bSearchRes = SearchData();
				if(f_bSearchRes==PACK_UNSEARCHED || f_bSearchRes==PACK_UNFINISHED)
					break;

				BYTE byType = DataExplain((const char*)m_FrameBuff, m_dwFrameLen,m_msgCache,msglen);
				if(byType != 0)
					NotifyMsg(byType,(char*)m_msgCache,msglen);							

			}while(f_bSearchRes==PACK_STILLDATA);

			nProcessLen += nThisLen;
		}

	}catch(...)
	{
		//AfxMessageBox(_T("FireException!"));
	}

	return 1;
}

int  CRdss2_1SearchExp::GetAccumulateCnt(UINT uMsgId)
{
	int nRslt = 0;

	if(m_mapProtCnt.count(uMsgId) > 0)
		nRslt = m_mapProtCnt[uMsgId];

	return nRslt;
}

void CRdss2_1SearchExp::CountRcvInfo(BYTE uInfoId)
{
	++m_mapProtCnt[uInfoId];
}

BYTE CRdss2_1SearchExp::SearchData()
{
	for(BYTE ch; m_dwCurrPt<m_dwComBufLen; m_dwCurrPt++)
	{
		ch = m_ComBuff[m_dwCurrPt];

		switch(m_bSearchStatus)
		{
		case SEARCHMACHINE_NULL:	//  0: $
			{
                static char Head[7];
				static int  HeadPtr = 0;
                Head[HeadPtr++]     = ch;
				if(HeadPtr == 6)
				{//搜索头部
					Head[6] = '\0';
					if(IsValidTitles(Head))
					{
						m_bSearchStatus = SEARCHMACHINE_LENGTH;
						m_bCheckSum  = 0;//不包括$.
						memcpy(m_FrameBuff,Head,6);

						m_bCheckSum ^= m_FrameBuff[1];
						m_bCheckSum ^= m_FrameBuff[2];
						m_bCheckSum ^= m_FrameBuff[3];
						m_bCheckSum ^= m_FrameBuff[4];
						m_bCheckSum ^= m_FrameBuff[5];

						if(memcmp(m_FrameBuff,"$BDXSD",6) == 0 || memcmp(m_FrameBuff,"$BDXST",6) == 0 )
							m_bBinaryMode = TRUE;
						else
							m_bBinaryMode = FALSE;

						m_dwFrameLen = 6;
						HeadPtr = 0;
						break;
					}else
					{
						int i = 1;
						for(; i<6;i++)
						{
							if(Head[i] == '$')
							{
								break;
							}
						}

						if(i<6)
						{
							for(int j = 0 ; j < 6-i ; j++)
							{
								Head[j] = Head[j+i];
							}

							HeadPtr -= i;
						}else
							HeadPtr  = 0;

						m_bSearchStatus = SEARCHMACHINE_NULL;
					}
				}
			}
			break;
		case SEARCHMACHINE_LENGTH:	//二进制协议获取长度

			m_FrameBuff[m_dwFrameLen]=ch;
			m_dwFrameLen++;

			if(m_dwFrameLen >= UNPACKBUFFSIZE-1)
			{
				m_bSearchStatus = SEARCHMACHINE_NULL;
				continue;
			}

			m_bCheckSum ^= ch;

			if(m_bBinaryMode && m_dwFrameLen == 8)//获取二进制的协议长度。
			{
				m_uBinaryLen = m_FrameBuff[6]*256 + m_FrameBuff[7];
				if(m_uBinaryLen >= UNPACKBUFFSIZE-1)
				{
					m_bSearchStatus = SEARCHMACHINE_NULL;
					continue;
				}else
				{
					m_bSearchStatus = SEARCHMACHINE_BINARYCHECK;
					continue;
				}
			}

			if(m_bBinaryMode == FALSE && ch == '*')
			{
                m_bSearchStatus = SEARCHMACHINE_IDENTIFIER;

				continue;
			}
			break;

		case SEARCHMACHINE_IDENTIFIER:	//  3: 数据和校验
			{
				m_FrameBuff[m_dwFrameLen] = ch;
				m_dwFrameLen ++;

				m_bCheckSum  ^= ch;
				if(m_dwFrameLen >= UNPACKBUFFSIZE-1)
				{
					m_bSearchStatus = SEARCHMACHINE_NULL;
					continue;
				}
				////end part..
				if(ch == '\n' && m_FrameBuff[m_dwFrameLen-2] =='\r' && m_FrameBuff[m_dwFrameLen-5] =='*')
				{
					int  byCheckAppendix;
					char chAppendix[3];

					chAppendix[0] = m_FrameBuff[m_dwFrameLen-4];
					chAppendix[1] = m_FrameBuff[m_dwFrameLen-3];
					chAppendix[2] = '\0';

					m_bCheckSum ^= 0x0D;
					m_bCheckSum ^= 0x0A;
					m_bCheckSum ^= '*';
					m_bCheckSum ^= chAppendix[0];
					m_bCheckSum ^= chAppendix[1];
                    sscanf(chAppendix,"%02x",&byCheckAppendix);

					if(byCheckAppendix!=m_bCheckSum)
					{
						m_bSearchStatus = SEARCHMACHINE_NULL;
						continue;
					}

					if(m_dwCurrPt==(m_dwComBufLen-1))
					{//找到完整的一帧数据,且缓冲区中只有一帧数据
						m_dwCurrPt=0;
						m_dwComBufLen=0;
						m_bSearchStatus=SEARCHMACHINE_NULL;		//added 2002-09-04 23:06
						m_FrameBuff[m_dwFrameLen] = '\0';
						return PACK_FINISHED;
					}
					else
					{//完整一帧数据取完后,还有其它帧的数据
						m_dwCurrPt++;
						m_bSearchStatus=SEARCHMACHINE_NULL;
						m_FrameBuff[m_dwFrameLen] = '\0';
						return PACK_STILLDATA;
					}
				}
			}
			break;
		case SEARCHMACHINE_BINARYCHECK://
			{//二进制协议算校验
				m_FrameBuff[m_dwFrameLen] = ch;
				m_dwFrameLen ++;
				m_bCheckSum ^= ch;
				if(m_dwFrameLen >= UNPACKBUFFSIZE-1)
				{
					m_bSearchStatus = SEARCHMACHINE_NULL;
					continue;
				}
				//,--m_dwFrameLen+1  == m_uBinaryLen--
				if(m_dwFrameLen  == m_uBinaryLen)//修改于2014-01-06 ,lxl
				{
					m_bCheckSum ^= '$';
					//m_bCheckSum ^= m_FrameBuff[m_dwFrameLen-1];
					if(0 == m_bCheckSum)
					{
						if(m_dwCurrPt==(m_dwComBufLen-1))
						{//找到完整的一帧数据,且缓冲区中只有一帧数据
							m_dwCurrPt=0;
							m_dwComBufLen=0;
							m_bSearchStatus=SEARCHMACHINE_NULL;		//added 2002-09-04 23:06
							return PACK_FINISHED;
						}
						else
						{//完整一帧数据取完后,还有其它帧的数据
							m_dwCurrPt++;
							m_bSearchStatus=SEARCHMACHINE_NULL;
							return PACK_STILLDATA;
						}
					}else
					{
						m_bSearchStatus = SEARCHMACHINE_NULL;
						continue;
					}
				}
			}
			break;
		default://默认状态
			m_bSearchStatus=SEARCHMACHINE_NULL;
		}
	}
	m_dwCurrPt=0;
	//一帧数据未接收完(SearchMachine=SEARCHMACHINE_LENGTH,下次进入时有用)
	if(m_bSearchStatus == SEARCHMACHINE_NULL)
	{
		return PACK_UNSEARCHED;
	}
	else
	{
        /*char chTitle[7];
		memcpy(chTitle,m_FrameBuff,6);
		chTitle[6] = '\0';
		if(!IsValidTitles(chTitle))
			m_bSearchStatus=SEARCHMACHINE_NULL;*/

		return PACK_UNFINISHED;
	}
}

BYTE CRdss2_1SearchExp::DataExplain(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	NULLReturn(DataBuff,CUNPACK_NULL) ;
	Len  = 0;
	//==========================信息类别判别==========================//
	/*for(iTitleIter=0; iTitleIter<RDSSCOUNT; iTitleIter++)
	{
	if(memcmp(byFrame+2+1, RDSSTitle[iTitleIter].cTitle, 3)==0)
	break;
	}
	TRUEReturn(iTitleIter==0 || iTitleIter >=RDSSCOUNT,CUNPACK_NULL);*/

	f_iInfoID = GetTitleID(byFrame);
	//============================信息解码============================//	
	switch(f_iInfoID)
	{
	case CUNPACK_KLT:
		{
			return explain_klt(byFrame,nFrameLen,DataBuff,Len);
		}
		break;
	case CUNPACK_ECT:
		{
			return explain_ect(byFrame,nFrameLen,DataBuff,Len);
		}
		break;
	case CUNPACK_GLL:
		{
			return explain_gll(byFrame,nFrameLen,DataBuff,Len);
		}
		break;
	case CUNPACK_GBS:
		{
			return explain_gbs(byFrame,nFrameLen,DataBuff,Len);
		}
		break;
	case CUNPACK_ZTG:
		{
			return explain_ztg(byFrame,nFrameLen,DataBuff,Len);
		}
		break;
	case CUNPACK_BSX:
		{
			return explain_bsx(byFrame,nFrameLen,DataBuff,Len);
		}
		break;
	case CUNPACK_BSQ:
		{
			return explain_bsq(byFrame,nFrameLen,DataBuff,Len);
		}
		break;
   //////////////////////////////////////////////////////////////////////////
   //BSD
   //////////////////////////////////////////////////////////////////////////
	case CUNPACK_BSD:
		{
			return explain_bsd(byFrame,nFrameLen,DataBuff,Len);
		}
		break;
	
	case CUNPACK_PKY:
		{
			return explain_pky(byFrame,nFrameLen,DataBuff,Len);
		}
			break;

	case CUNPACK_ICM:
		{
			return explain_icm(byFrame,nFrameLen,DataBuff,Len);
		}
		break;
	//////////////////////////////////////////////////////////////////////////
		//PMU
   //////////////////////////////////////////////////////////////////////////
	case CUNPACK_PMU:
		{
			return explain_pmu(byFrame,nFrameLen,DataBuff,Len);
		}
		break;
		
	case CUNPACK_GSV:
		{			
			return explain_gsv(byFrame,nFrameLen,DataBuff,Len);
		}
		break;
		
	case CUNPACK_RMC:
		{
			return explain_rmc(byFrame,nFrameLen,DataBuff,Len);
		}
		break;		
	case CUNPACK_GGA:
		{
			return explain_gga(byFrame,nFrameLen,DataBuff,Len);
		}
		break;
		//////////////////////////////////////////////////////////////////////////
		//explain..
		//
	    //////////////////////////////////////////////////////////////////////////
	case CUNPACK_BSI:
		{
			return explain_bsi(byFrame,nFrameLen,DataBuff,Len);
		}
		break;		
	case CUNPACK_DWR:
		{
			return explain_dwr(byFrame,nFrameLen,DataBuff,Len);
		}
		break;

	case CUNPACK_FKI:
		{
			return explain_fki(byFrame,nFrameLen,DataBuff,Len);
		}
		break;
		
	case CUNPACK_ICI:
		{
			return explain_ici(byFrame,nFrameLen,DataBuff,Len);
		}
		break;
		
	case CUNPACK_ICZ:
		{
			return explain_icz(byFrame,nFrameLen,DataBuff,Len);
		}
		break;
		
	case CUNPACK_KLS:
 		{
 			return explain_kls(byFrame,nFrameLen,DataBuff,Len);
 		}
		break;
		
	case CUNPACK_HZR:
		{
			return explain_hzr(byFrame,nFrameLen,DataBuff,Len);
		}
		break;

	case CUNPACK_TXR:
		{
			return explain_txr(byFrame,nFrameLen,DataBuff,Len);
		}
		break;		
	case CUNPACK_LZM:
		{
			return explain_lzm(byFrame,nFrameLen,DataBuff,Len);
		}
		break;		
	case CUNPACK_WAA:
		{
			return explain_waa(byFrame,nFrameLen,DataBuff,Len);
		}
		break;

	case CUNPACK_SBX://2014-01-09
		{
			return explain_sbx(byFrame,nFrameLen,DataBuff,Len);
		}
		break;

	case CUNPACK_ZTI:
		{
           return explain_zti(byFrame,nFrameLen,DataBuff,Len);
		}
		break;		
	case CUNPACK_ZDA:
		{
			return explain_zda(byFrame,nFrameLen,DataBuff,Len);
		}
		break;
		//////////////////////////////////////////////////////////////////////////
		//explain..
		//
	    //////////////////////////////////////////////////////////////////////////
	case CUNPACK_GSA:
		{
			return explain_gsa(byFrame,nFrameLen,DataBuff,Len);
		}
		break;
		//////////////////////////////////////////////////////////////////////////
		//explain..
		//
	    //////////////////////////////////////////////////////////////////////////
	case CUNPACK_GXM:
		{
			return explain_gxm(byFrame,nFrameLen,DataBuff,Len);
		}
		break;

	case CUNPACK_DHV:
		{
			return explain_dhv(byFrame,nFrameLen,DataBuff,Len);
		}
		break;		
		/*case CUNPACK_XSD:
		{
		return explain_xsd(byFrame,nFrameLen,DataBuff,Len);
		}
		break;*/

	case CUNPACK_XST://0107
		{
			return explain_xst(byFrame,nFrameLen,DataBuff,Len);
		}
		break;		
	case CUNPACK_RNS:
		{
			return explain_rns(byFrame,nFrameLen,DataBuff,Len);
		}
		break;
		
	case CUNPACK_APL:
		{
			return explain_apl(byFrame,nFrameLen,DataBuff,Len);
		}
		break;

	case CUNPACK_BID:
		{
			return explain_bid(byFrame,nFrameLen,DataBuff,Len);
		}
		break;
		
	case CUNPACK_ZHR:
		{
			return explain_zhr(byFrame,nFrameLen,DataBuff,Len);
		}
		break;

	default:
		break;
	}//switch

	return f_iInfoID;
}

BYTE CRdss2_1SearchExp::ComputeCheckSumRdss21(BYTE *Buff, int Len)
{
	BYTE f_bCheckSum = 0;
	for(int i=0; i<Len; i++)
	{
		//','不参与校验计算
		f_bCheckSum ^= Buff[i];
	}
	return f_bCheckSum;
}

int CRdss2_1SearchExp::explain_klt(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	string strInfo;
	int nPos = 0  ;
	LPKLTInfo lpKltInfo  =(LPKLTInfo)DataBuff;
	memset(lpKltInfo,0,sizeof(KLTInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		lpKltInfo->chCharacter = f_szTemp2[1];
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL)  ;
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		lpKltInfo->nUsrId = atoi(f_szTemp2+1);
	}			

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		lpKltInfo->byCommType = atoi(f_szTemp2 +1);
		if(!CheckCloseRange(lpKltInfo->byCommType,0,2))
			return CUNPACK_NULL;
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		strInfo = f_szTemp2 +1   ;
		nPos  = strInfo.find('*');
		FALSEReturn(nPos != string::npos,CUNPACK_NULL);
		FALSEReturn(nPos  < sizeof(lpKltInfo->chContent)-1,CUNPACK_NULL);
		strcpy(lpKltInfo->chContent,strInfo.substr(0,nPos).c_str());
	}

	Len = sizeof(KLTInfo);

	return CUNPACK_KLT;

}

int CRdss2_1SearchExp::explain_ect(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	size_t nPos = 0;
	int    nCnt = 0;
	string strInfo;

	LPECTInfo lpECTInfo = (LPECTInfo)DataBuff;
	memset(lpECTInfo,0,sizeof(ECTInfo));

	FRAME_SYS((char*)byFrame,lpECTInfo->chSysType,sizeof(lpECTInfo->chSysType));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2 +1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		lpECTInfo->bySatNo = atoi(f_szTemp2 +1);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		strInfo = f_szTemp2 + 1;
		nPos    = strInfo.find(',');
		FALSEReturn(nPos!=string::npos,CUNPACK_NULL);
		FALSEReturn(nPos+1 < sizeof(lpECTInfo->chFreq),CUNPACK_NULL);
		strcpy(lpECTInfo->chFreq,strInfo.substr(0,nPos).c_str());
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
		lpECTInfo->byChannelNo = atoi(f_szTemp2 +1);

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		lpECTInfo->chRoute = f_szTemp2[1];
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		strInfo = f_szTemp2+1;
		nPos = strInfo.find('*');
		FALSEReturn(nPos < sizeof(lpECTInfo->chTeleInfo)-1,CUNPACK_NULL);
		strcpy(lpECTInfo->chTeleInfo,strInfo.substr(0,nPos).c_str());
	}

	Len = sizeof(ECTInfo);

	return CUNPACK_ECT;

}

int CRdss2_1SearchExp::explain_gll(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	string    strInfo;
	LPGLLInfo pGLLInfo = (LPGLLInfo)DataBuff;
	memset(pGLLInfo,0,sizeof(GLLInfo));

	FRAME_SYS((char*)byFrame,pGLLInfo->chSysType,sizeof(pGLLInfo->chSysType));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		strInfo = f_szTemp2 +1;

		BOOL bCheck = FALSE;

		FALSEReturn(CheckDouble(f_szTemp2+1),CUNPACK_NULL)
			FALSEReturn(IntpartLen(f_szTemp2+1)==4,CUNPACK_NULL);

		FALSEReturn(strInfo.size() > 1, CUNPACK_NULL);//
		pGLLInfo->dfLat = atoi(strInfo.substr(0,2).c_str());
		const char * pstr = strInfo.c_str();
		double dfData = atof(pstr+2);
		FALSEReturn(CheckLCloseRange(dfData,0,60),CUNPACK_NULL);
		pGLLInfo->dfLat+= dfData/60;

		bCheck = CheckCloseRange(pGLLInfo->dfLat,0,90);
		FALSEReturn(bCheck,CUNPACK_NULL);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		FALSEReturn(f_szTemp2[1] == 'N' || f_szTemp2[1] == 'S',CUNPACK_NULL);
		pGLLInfo->chLatNS = f_szTemp2[1];
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;				
		FALSEReturn(CheckDouble(f_szTemp2+1),CUNPACK_NULL);
		FALSEReturn(IntpartLen(f_szTemp2+1) ==5,CUNPACK_NULL);
		strInfo = f_szTemp2 +1;
		FALSEReturn(strInfo.size() > 2, CUNPACK_NULL);
		pGLLInfo->dfLon = atoi(strInfo.substr(0,3).c_str());
		const char * pstr = strInfo.c_str();
		double dfData = atof(pstr+3);
		FALSEReturn(CheckLCloseRange(dfData,0,60),CUNPACK_NULL);
		pGLLInfo->dfLon += dfData/60;

		bCheck = CheckLCloseRange(pGLLInfo->dfLon,0,180);
		FALSEReturn(bCheck,CUNPACK_NULL);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		FALSEReturn(f_szTemp2[1] == 'E' || f_szTemp2[1] == 'W',CUNPACK_NULL);
		pGLLInfo->chLonEW = f_szTemp2[1];
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{

		BOOL bCheck = FALSE;
		int  nFieldLen = FieldLength(f_szTemp2+1);//
		bCheck = (nFieldLen > 5);
		FALSEReturn(bCheck,CUNPACK_NULL);

		if(CheckDouble(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;

		if(nFieldLen == 6)
			FALSEReturn(CheckInt(f_szTemp2 +1),CUNPACK_NULL);

		strInfo = f_szTemp2 +1;
		FALSEReturn(strInfo.size()>3,CUNPACK_NULL);
		pGLLInfo->wPosHour   = atoi(strInfo.substr(0,2).c_str());
		pGLLInfo->wPosMinute = atoi(strInfo.substr(2,2).c_str());
		const char * pstr = strInfo.c_str();
		pGLLInfo->fPosSecond = atof(pstr+4);

		bCheck = CheckCloseRange(pGLLInfo->wPosHour,0,23);
		FALSEReturn(bCheck,CUNPACK_NULL);

		bCheck = CheckCloseRange(pGLLInfo->wPosMinute,0,59);
		FALSEReturn(bCheck,CUNPACK_NULL);

		bCheck = CheckLCloseRange(pGLLInfo->fPosSecond,0,60);
		FALSEReturn(bCheck,CUNPACK_NULL);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		FALSEReturn(f_szTemp2[1] == 'A' || f_szTemp2[1] == 'V',CUNPACK_NULL);
		pGLLInfo->chDataStatus = f_szTemp2[1];
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ','&&f_szTemp2[1] != '*')
	{
		BOOL bCheck = CheckInt(f_szTemp2 +1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pGLLInfo->byModeIndicate = atoi(f_szTemp2 +1);
		if(!CheckCloseRange(pGLLInfo->byModeIndicate,0,5))
			return CUNPACK_NULL;
	}

	Len = sizeof(GLLInfo);

	return CUNPACK_GLL;
}

int CRdss2_1SearchExp::explain_gbs(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	string strInfo;
	LPGBSInfo pGBSInfo = (LPGBSInfo)DataBuff;
	memset(pGBSInfo,0,sizeof(GBSInfo));

	FRAME_SYS((char*)byFrame,pGBSInfo->chSysType,sizeof(pGBSInfo->chSysType));
	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;				

		strInfo = f_szTemp2 + 1;
		FALSEReturn(strInfo.size() >3 ,CUNPACK_NULL);

		pGBSInfo->wPosHour   = atoi(strInfo.substr(0,2).c_str());
		pGBSInfo->wPosMinute = atoi(strInfo.substr(2,2).c_str());
		const char * pstr = strInfo.c_str();
		pGBSInfo->fPosSecond = atoi(pstr+4);
		bCheck = CheckHMSTime(pGBSInfo->wPosHour,pGBSInfo->wPosMinute,pGBSInfo->fPosSecond);
		FALSEReturn(bCheck,CUNPACK_NULL);
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2 +1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pGBSInfo->fLatPredicErr = atof(f_szTemp2 +1);
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2 +1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pGBSInfo->fLonPredicErr = atof(f_szTemp2 +1);
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2 +1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pGBSInfo->fHgtPredicErr = atof(f_szTemp2 +1);
	}


	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2 +1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		pGBSInfo->byFailedSat = atoi(f_szTemp2 +1);
		FALSEReturn(pGBSInfo->byFailedSat>0&&pGBSInfo->byFailedSat<34,CUNPACK_NULL);
	}	
	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2 +1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pGBSInfo->fProbability = atof(f_szTemp2 +1);
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2 +1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pGBSInfo->fPredictSatCauseErr = atof(f_szTemp2 +1);
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2 +1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pGBSInfo->fPredictSatCauseErr = atof(f_szTemp2 +1);
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ','&& f_szTemp2[1] != '*')
	{
		BOOL bCheck = CheckDouble(f_szTemp2 +1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pGBSInfo->fPredictStdErr = atof(f_szTemp2 +1);
	}

	Len = sizeof(GBSInfo);

	return CUNPACK_GBS;
}

int CRdss2_1SearchExp::explain_ztg(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	LPVTGInfo  pVTGInfo = (LPVTGInfo)DataBuff;
	memset(pVTGInfo,0,sizeof(VTGInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2 +1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pVTGInfo->fGrndDirNorth = atof(f_szTemp2 +1);
	}

	f_szTemp2 = strstr(f_szTemp2 +1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		pVTGInfo->chNorthRef = f_szTemp2[1];
	}

	f_szTemp2 = strstr(f_szTemp2 +1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2 +1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pVTGInfo->fGrndDirMagNorth = atof(f_szTemp2 +1);
	}

	f_szTemp2 = strstr(f_szTemp2 +1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		pVTGInfo->chMagNorthRef = f_szTemp2[1];
	}

	f_szTemp2 = strstr(f_szTemp2 +1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2 +1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pVTGInfo->fGrndSpeedN = atof(f_szTemp2 +1);
	}

	f_szTemp2 = strstr(f_szTemp2 +1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		pVTGInfo->chSpeedUnitN = f_szTemp2[1];
	}

	f_szTemp2 = strstr(f_szTemp2 +1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2 +1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pVTGInfo->fGrndSpeedKm = atof(f_szTemp2 +1);
	}

	f_szTemp2 = strstr(f_szTemp2 +1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		pVTGInfo->chSpeedUnitK = f_szTemp2[1];
	}

	f_szTemp2 = strstr(f_szTemp2 +1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',' && f_szTemp2[1] != '*')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		FALSEReturn(f_szTemp2[1] == 'A' || f_szTemp2[1] == 'B'|| f_szTemp2[1] == 'E'|| f_szTemp2[1] == 'M'|| f_szTemp2[1] == 'S'|| f_szTemp2[1] == 'N',CUNPACK_NULL);
		pVTGInfo->chModeIndicate = f_szTemp2[1];
	}

	Len = sizeof(VTGInfo);

	return CUNPACK_ZTG;
}

int CRdss2_1SearchExp::explain_bsx(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	int nCnt = 0  ;
	string strInfo;
	char * f_szTemp1 = NULL;
	LPBSXInfo pBSXInfo = (LPBSXInfo)DataBuff;
	memset(pBSXInfo,0,sizeof(pBSXInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		f_szTemp1 = f_szTemp2 +1;
		while(*f_szTemp1 != '*' && nCnt < sizeof(pBSXInfo->chName)-1)
		{
			pBSXInfo->chName[nCnt] = *f_szTemp1;
			nCnt ++;
		}
	}

	f_szTemp2 = strstr(f_szTemp2 + 1 , ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2 +1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pBSXInfo->wTotalSentence = atoi(f_szTemp2 + 1);
	}

	f_szTemp2 = strstr(f_szTemp2 + 1 , ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2 +1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pBSXInfo->wCurrSentenceNo = atoi(f_szTemp2 + 1);
	}

	f_szTemp2 = strstr(f_szTemp2 + 1 , ",");//
	NULLReturn(f_szTemp2,CUNPACK_NULL);//
	if(f_szTemp2[1] != ',')//
	{//
		BOOL bCheck = CheckInt(f_szTemp2 +1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		nCnt = atoi(f_szTemp2 + 1);
		nCnt = MAX2(nCnt,sizeof(pBSXInfo->posInfo)/sizeof(PositionInfo));
		pBSXInfo->wPointNum = nCnt;
	}

	for(int i = 0 ; i < nCnt ; i++)
	{//
		f_szTemp2 = strstr(f_szTemp2 + 1, ",");
		NULLReturn(f_szTemp2,CUNPACK_NULL);
		if(f_szTemp2[1] != ',')
		{
			BOOL bCheck = FALSE;					

			strInfo = f_szTemp2 + 1;
			FALSEReturn(strInfo.size() >1 ,CUNPACK_NULL);

			pBSXInfo->posInfo[i].dfLat  = atoi(strInfo.substr(0,2).c_str());
			const char* pstr = strInfo.c_str();
			pBSXInfo->posInfo[i].dfLat += atof(pstr+2)/60.0;

			bCheck = CheckCloseRange(pBSXInfo->posInfo[i].dfLat,0,90);
			FALSEReturn(bCheck,CUNPACK_NULL);
		}

		f_szTemp2 = strstr(f_szTemp2 + 1, ",");
		NULLReturn(f_szTemp2,CUNPACK_NULL);
		if(f_szTemp2[1] != ',')
		{
			BOOL bCheck = FALSE;
			bCheck = (FieldLength(f_szTemp2+1) == 1);
			FALSEReturn(bCheck,CUNPACK_NULL);

			FALSEReturn(f_szTemp2[1] == 'N' || f_szTemp2[1] == 'S',CUNPACK_NULL);
			pBSXInfo->posInfo[i].chLatNS = f_szTemp2[1];
		}

		f_szTemp2 = strstr(f_szTemp2 + 1, ",");
		NULLReturn(f_szTemp2,CUNPACK_NULL);
		if(f_szTemp2[1] != ',')
		{
			BOOL bCheck = FALSE;					

			strInfo = f_szTemp2 +1;
			pBSXInfo->posInfo[i].dfLong  = atoi(strInfo.substr(0,3).c_str());
			const char *pstr = strInfo.c_str();
			pBSXInfo->posInfo[i].dfLong += atof(pstr+3)/60.0;

			bCheck = CheckLCloseRange(pBSXInfo->posInfo[i].dfLong,0,180);
			FALSEReturn(bCheck,CUNPACK_NULL);
		}

		f_szTemp2 = strstr(f_szTemp2 + 1, ",");
		if(f_szTemp2[1] != ',')
		{
			BOOL bCheck = FALSE;
			bCheck = (FieldLength(f_szTemp2+1) == 1);
			FALSEReturn(bCheck,CUNPACK_NULL);

			FALSEReturn(f_szTemp2[1] == 'E' || f_szTemp2[1] == 'W',CUNPACK_NULL);
			pBSXInfo->posInfo[i].chLongEW = f_szTemp2[1];
		}

		f_szTemp2 = strstr(f_szTemp2 + 1, ",");
		NULLReturn(f_szTemp2,CUNPACK_NULL);
		if(f_szTemp2[1] != ',')
		{
			BOOL bCheck = CheckDouble(f_szTemp2 +1);
			FALSEReturn(bCheck,CUNPACK_NULL);
			pBSXInfo->posInfo[i].dfHgt = atoi(f_szTemp2 +1);
		}

		f_szTemp2 = strstr(f_szTemp2 + 1, ",");
		NULLReturn(f_szTemp2,CUNPACK_NULL);
		if(f_szTemp2[1] != ',' && f_szTemp2[1] != '*')
			pBSXInfo->posInfo[i].chHgtUnit = f_szTemp2[1];
	}
	Len = sizeof(BSXInfo);

	return CUNPACK_BSX;
}

int CRdss2_1SearchExp::explain_bsq(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	int nCnt = 0  ;
	string strInfo;
	char *f_szTemp1 = NULL;

	LPBSQInfo pBSQInfo = (LPBSQInfo)DataBuff;
	memset(pBSQInfo,0,sizeof(BSQInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		f_szTemp1 = f_szTemp2 +1;
		while(*f_szTemp1 != '*' && nCnt < sizeof(pBSQInfo->chName)-1)
		{
			pBSQInfo->chName[nCnt] = *f_szTemp1;
			nCnt ++;
		}
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");			
	if(f_szTemp2[1] != ',')
	{
		strInfo = f_szTemp2 + 1;
		FALSEReturn(strInfo.size() > 11,CUNPACK_NULL);

		pBSQInfo->dfCenterLat = atoi(strInfo.substr(0,2).c_str());
		pBSQInfo->dfCenterLat+= atof(strInfo.substr(2,10).c_str())/60.0;
		BOOL bCheck = CheckCloseRange(pBSQInfo->dfCenterLat,0,90);
		FALSEReturn(bCheck,CUNPACK_NULL);
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		FALSEReturn(f_szTemp2[1] == 'N' || f_szTemp2[1] == 'S',CUNPACK_NULL);
		pBSQInfo->chLatNS = f_szTemp2[1];
	}


	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);			
	if(f_szTemp2[1] !=',')
	{
		strInfo = f_szTemp2 + 1;
		FALSEReturn(strInfo.size() > 12,CUNPACK_NULL);

		pBSQInfo->dfCenterLong = atoi(strInfo.substr(0,3).c_str());
		pBSQInfo->dfCenterLong+= atof(strInfo.substr(3,10).c_str())/60.0;
		BOOL bCheck = CheckLCloseRange(pBSQInfo->dfCenterLong,0,180);
		FALSEReturn(bCheck,CUNPACK_NULL);
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		FALSEReturn(f_szTemp2[1] == 'E' || f_szTemp2[1] == 'W',CUNPACK_NULL);
		pBSQInfo->chLongEW = f_szTemp2[1];
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2 + 1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pBSQInfo->dfRadius = atof(f_szTemp2 + 1);
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',' &&f_szTemp2[1] != '*')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		pBSQInfo->chRadiusUnit = f_szTemp2[1];
	}

	Len = sizeof(BSQInfo);

	return CUNPACK_BSQ;

}

int CRdss2_1SearchExp::explain_bsd(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	int nCnt = 0  ;
	string strInfo;
	char *f_szTemp1 = NULL;

	LPBSDInfo pBSDInfo = (LPBSDInfo)DataBuff;
	memset(pBSDInfo,0,sizeof(BSDInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		f_szTemp1 = f_szTemp2 +1;
		while(*f_szTemp1 != '*' && nCnt < sizeof(pBSDInfo->chName)-1)
		{
			pBSDInfo->chName[nCnt] = *f_szTemp1;
			nCnt ++;
		}
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		f_szTemp1 = f_szTemp2 +1;
		nCnt = 0;
		while(*f_szTemp1 != '*' && nCnt < sizeof(pBSDInfo->chMapName)-1)
		{
			pBSDInfo->chMapName[nCnt] = *f_szTemp1;
			nCnt ++;
		}
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);

	if(f_szTemp2[1] != ',')
	{
		strInfo = f_szTemp2 +1;
		FALSEReturn(strInfo.size()>11,CUNPACK_NULL);

		pBSDInfo->dfLat = atoi(strInfo.substr(0,2).c_str());
		pBSDInfo->dfLat += atof(strInfo.substr(2,10).c_str())/60.0;

		BOOL bCheck = CheckCloseRange(pBSDInfo->dfLat,0,90);
		FALSEReturn(bCheck,CUNPACK_NULL);
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		FALSEReturn(f_szTemp2[1] == 'N' || f_szTemp2[1] == 'S',CUNPACK_NULL);
		pBSDInfo->chLatNS = f_szTemp2[1];
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);

	if(f_szTemp2[1] != ',')
	{
		strInfo = f_szTemp2 +1;
		FALSEReturn(strInfo.size() > 12,CUNPACK_NULL);
		pBSDInfo->dfLong = atoi(strInfo.substr(0,3).c_str());
		pBSDInfo->dfLong += atof(strInfo.substr(3,10).c_str())/60.0;
		BOOL bCheck = CheckLCloseRange(pBSDInfo->dfLong,0,180);
		FALSEReturn(bCheck,CUNPACK_NULL);
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		FALSEReturn(f_szTemp2[1] == 'E' || f_szTemp2[1] == 'W',CUNPACK_NULL);
		pBSDInfo->chLongEW = f_szTemp2[1];
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1]!=',')
	{
		pBSDInfo->dfHgt = atoi(f_szTemp2 +1);
	}

	f_szTemp2 = strstr(f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1]!=','&&f_szTemp2[1] != '*')
		pBSDInfo->chHgtUnit = f_szTemp2[1];

	Len = sizeof(BSDInfo);

	return CUNPACK_BSD;
}

int CRdss2_1SearchExp::explain_pky(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	int nCnt = 0;
	char *f_szTemp1 = NULL;
	LPPKYInfo pPKYInfo = (LPPKYInfo)DataBuff;

	memset(pPKYInfo,0,sizeof(PKYInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] !=',')
		pPKYInfo->uWeek = atoi(f_szTemp2 +1);

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1]!=',')
	{
		f_szTemp1 = f_szTemp2 + 1;
		while(*f_szTemp1 !=',' && nCnt < (sizeof(pPKYInfo->chSecretKey)-1))
		{
			pPKYInfo->chSecretKey[nCnt] = *f_szTemp1;
			f_szTemp1 ++;
			nCnt++;
		}
		pPKYInfo->chSecretKey[nCnt] = '\0';
	}						

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		f_szTemp1 = f_szTemp2 + 1;
		nCnt = 0;
		while(*f_szTemp1 !=',' && nCnt < (sizeof(pPKYInfo->chSecretKey) -1))
		{
			pPKYInfo->chKey[nCnt] = *f_szTemp1;
			f_szTemp1 ++;
			nCnt++;
		}
		pPKYInfo->chKey[nCnt] = '\0';
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		f_szTemp1 = f_szTemp2 + 1;
		nCnt = 0;
		while(*f_szTemp1 !=',' && nCnt < (sizeof(pPKYInfo->chSecretKey) -1))
		{
			pPKYInfo->chB1Param[nCnt] = *f_szTemp1;
			f_szTemp1 ++;
			nCnt++;
		}
		pPKYInfo->chB1Param[nCnt]  = '\0';
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		f_szTemp1 = f_szTemp2 + 1;
		nCnt = 0;
		while(*f_szTemp1 !=',' && nCnt < (sizeof(pPKYInfo->chSecretKey) -1))
		{
			pPKYInfo->chB2Param[nCnt] = *f_szTemp1;
			f_szTemp1 ++;
			nCnt++;
		}
		pPKYInfo->chB2Param[nCnt] = '\0';
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		f_szTemp1 = f_szTemp2 + 1;
		nCnt = 0;
		while((*f_szTemp1 !=',')&& (*f_szTemp1 !='*') && nCnt < (sizeof(pPKYInfo->chSecretKey) -1))
		{
			pPKYInfo->chB3Param[nCnt] = *f_szTemp1;
			f_szTemp1 ++;
			nCnt++;
		}
		pPKYInfo->chB3Param[nCnt] = '\0';
	}

	Len = sizeof(PKYInfo);

	return CUNPACK_PKY;
}

int CRdss2_1SearchExp::explain_icm(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	string strDate;
	LPICMInfo pICMInfo = (LPICMInfo)DataBuff;
	memset(pICMInfo,0,sizeof(ICMInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");

	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ','&&f_szTemp2[1] != '*')
	{
		BOOL bCheck = FALSE;
		int  nFieldLen = FieldLength(f_szTemp2+1);
		bCheck = (nFieldLen==6);
		FALSEReturn(bCheck,CUNPACK_NULL);

		if(CheckInt(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;

		strDate = f_szTemp2 + 1;
		FALSEReturn(strDate.size() > 5,CUNPACK_NULL);
		pICMInfo->wEndDay   = atoi(strDate.substr(0,2).c_str());
		pICMInfo->wEndMonth = atoi(strDate.substr(2,2).c_str());
		pICMInfo->wEndYear   = atoi(strDate.substr(4,2).c_str());

		FALSEReturn(pICMInfo->wEndYear <51,CUNPACK_NULL);
		FALSEReturn(pICMInfo->wEndDay  < 32,CUNPACK_NULL);
		FALSEReturn(pICMInfo->wEndMonth  < 13,CUNPACK_NULL);
	}

	Len = sizeof(ICMInfo);

	return CUNPACK_ICM;
}

int CRdss2_1SearchExp::explain_pmu(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	string strDate;
	LPPMUInfo pPmuInfo = (LPPMUInfo)DataBuff;
	memset(pPmuInfo,0,sizeof(PMUInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] !=',')
	{
		BOOL bCheck = CheckInt(f_szTemp2 + 1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pPmuInfo->byFreq = atoi(f_szTemp2 + 1);


		if(CheckCloseRange(pPmuInfo->byFreq,1,3)!= TRUE)
			return CUNPACK_NULL;
	}			

	f_szTemp2 = strstr((char*)f_szTemp2 + 1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ','&&f_szTemp2[1] != '*')
	{
		strDate = f_szTemp2 +1;

		int  nFieldLen = FieldLength(f_szTemp2+1);
		BOOL bCheck = (nFieldLen==6);
		FALSEReturn(bCheck,CUNPACK_NULL);

		if(CheckInt(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;

		pPmuInfo->wEndDay   = atoi(strDate.substr(0,2).c_str());
		pPmuInfo->wEndMonth = atoi(strDate.substr(2,2).c_str());
		pPmuInfo->wEndYear  = atoi(strDate.substr(4,2).c_str());

		FALSEReturn(pPmuInfo->wEndYear <51,CUNPACK_NULL);
		FALSEReturn(pPmuInfo->wEndDay  < 32,CUNPACK_NULL);
		FALSEReturn(pPmuInfo->wEndMonth  < 13,CUNPACK_NULL);
	}

	Len = sizeof(PMUInfo);

	return CUNPACK_PMU;
}

int CRdss2_1SearchExp::explain_gsv(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{	
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	static int nTotalSentence   = 0;
	static int nCurrentSentence = 0;
	string strInfo;
	int    nSatCnt = 0;
	int    nCommaCnt = 0;

	LPGSVInfo lpGsvInfo = (LPGSVInfo)DataBuff;
	memset(lpGsvInfo,0,sizeof(GSVInfo));

	FRAME_SYS((char*)byFrame,lpGsvInfo->chSys,sizeof(lpGsvInfo->chSys));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2 +1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		nTotalSentence = atoi(f_szTemp2 +1 );

		if(CheckCloseRange(nTotalSentence,1,3)!= TRUE)
			return CUNPACK_NULL;
	}

	f_szTemp2 = strstr(f_szTemp2 +1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);

	if(f_szTemp2[1] !=',')
	{
		BOOL bCheck = CheckInt(f_szTemp2 +1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		nCurrentSentence = atoi(f_szTemp2 +1 );
		if(CheckCloseRange(nCurrentSentence,1,3)!= TRUE)
			return CUNPACK_NULL;
	}
	f_szTemp2 = strstr(f_szTemp2 +1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1]!=',')
	{
		BOOL bCheck = CheckInt(f_szTemp2 +1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		lpGsvInfo->nSatVisible = atoi(f_szTemp2 +1 );

	}

	char * pStr = f_szTemp2 +1;
	while((*pStr)!= '*' && (nCommaCnt/4)< 5)
	{
		if((*pStr) == ',')
			nCommaCnt ++;
		pStr ++;
	}

	//ASSERT(nCommaCnt %4 == 0);
	FALSEReturn(nCommaCnt%4==0,CUNPACK_NULL);
	nSatCnt = nCommaCnt/4;
	FALSEReturn(nSatCnt<5 , CUNPACK_NULL);

	for(int i = 0 ; i < nSatCnt && i < 4; i++)
	{
		f_szTemp2 = strstr(f_szTemp2 +1, ",");
		NULLReturn(f_szTemp2,CUNPACK_NULL);
		if(f_szTemp2[1]!=',')
		{
			BOOL bCheck = CheckInt(f_szTemp2 +1);
			FALSEReturn(bCheck,CUNPACK_NULL);
			lpGsvInfo->satInfo[i].nSatNo      = atoi(f_szTemp2 +1);
		}

		f_szTemp2 = strstr(f_szTemp2 +1, ",");
		NULLReturn(f_szTemp2,CUNPACK_NULL);

		if(f_szTemp2[1]!=',')
		{
			BOOL bCheck = CheckDouble(f_szTemp2 +1);
			FALSEReturn(bCheck,CUNPACK_NULL);
			lpGsvInfo->satInfo[i].fElevation  = atof(f_szTemp2 +1);
			bCheck = CheckRCloseRange(lpGsvInfo->satInfo[i].fElevation,0,90);
			FALSEReturn(bCheck,CUNPACK_NULL);
		}

		f_szTemp2 = strstr(f_szTemp2 +1, ",");
		NULLReturn(f_szTemp2,CUNPACK_NULL);
		if(f_szTemp2[1]!=',')
		{
			BOOL bCheck = CheckDouble(f_szTemp2 +1);
			FALSEReturn(bCheck,CUNPACK_NULL);
			lpGsvInfo->satInfo[i].fOrientAngle = atof(f_szTemp2 +1);
			bCheck = CheckCloseRange(lpGsvInfo->satInfo[i].fOrientAngle,-360,360);
			FALSEReturn(bCheck,CUNPACK_NULL);
		}

		f_szTemp2 = strstr(f_szTemp2 +1, ",");
		NULLReturn(f_szTemp2,CUNPACK_NULL);
		if(f_szTemp2[1]!=',')
		{
			BOOL bCheck = CheckDouble(f_szTemp2 +1);
			FALSEReturn(bCheck,CUNPACK_NULL);
			lpGsvInfo->satInfo[i].fSnr = atof(f_szTemp2 +1);
			bCheck = CheckCloseRange(lpGsvInfo->satInfo[i].fSnr,0,100);
			FALSEReturn(bCheck,CUNPACK_NULL);
		}
	}

	lpGsvInfo->nTotalStence = nTotalSentence;
	lpGsvInfo->nCurrStence  = nCurrentSentence;
	if(nTotalSentence < nCurrentSentence)
	{
		lpGsvInfo->nTotalStence = nCurrentSentence;
		lpGsvInfo->nCurrStence  = nTotalSentence;
	}

	lpGsvInfo->nCurrSatNum  = nSatCnt;
	Len = sizeof(GSVInfo);

	return CUNPACK_GSV;
}

int CRdss2_1SearchExp::explain_rmc(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	string strInfo;
	LPRMCInfo lpRmcInfo = (LPRMCInfo)DataBuff;
	memset(lpRmcInfo,0,sizeof(RMCInfo));

	FRAME_SYS((char*)byFrame,lpRmcInfo->chSys,sizeof(lpRmcInfo->chSys));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);

	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		int  nFieldLen = FieldLength(f_szTemp2+1);
		bCheck = (nFieldLen > 5);
		FALSEReturn(bCheck,CUNPACK_NULL);

		if(nFieldLen == 6)
			FALSEReturn(CheckInt(f_szTemp2+1),CUNPACK_NULL);

		if(CheckDouble(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;

		strInfo     = f_szTemp2 +1;
		FALSEReturn(strInfo.size() > 3,CUNPACK_NULL);
		lpRmcInfo->nPosHour   = atoi(strInfo.substr(0,2).c_str());
		bCheck = CheckCloseRange(lpRmcInfo->nPosHour,0,23);
		FALSEReturn(bCheck,CUNPACK_NULL);

		lpRmcInfo->nPosMinute = atoi(strInfo.substr(2,2).c_str());
		bCheck = CheckCloseRange(lpRmcInfo->nPosMinute,0,59);
		FALSEReturn(bCheck,CUNPACK_NULL);

		const char * pstr = strInfo.c_str() ;
		lpRmcInfo->fPosSecond = atof(pstr+4);
		bCheck = CheckLCloseRange(lpRmcInfo->fPosSecond,0,60);
		FALSEReturn(bCheck,CUNPACK_NULL);
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		lpRmcInfo->byPosStatus = f_szTemp2[1];
		FALSEReturn(lpRmcInfo->byPosStatus == 'A' ||  lpRmcInfo->byPosStatus == 'V' , CUNPACK_NULL);
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);

	if(f_szTemp2[1] != ',')
	{
		FALSEReturn(CheckDouble(f_szTemp2+1),CUNPACK_NULL);
		FALSEReturn(IntpartLen (f_szTemp2+1)==4 ,CUNPACK_NULL);

		strInfo = f_szTemp2 +1;
		FALSEReturn(strInfo.size() > 1,CUNPACK_NULL);
		lpRmcInfo->dfLat =  atoi(strInfo.substr(0,2).c_str());

		const char* pstr = strInfo.c_str();
		double dfData = atof(pstr + 2);
		FALSEReturn(CheckLCloseRange(dfData,0,60),CUNPACK_NULL);
		lpRmcInfo->dfLat += dfData/60.0;

		BOOL bCheck = CheckCloseRange(lpRmcInfo->dfLat,0,90);
		FALSEReturn(bCheck,CUNPACK_NULL);

	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		FALSEReturn(FieldLength(f_szTemp2+1) == 1,CUNPACK_NULL);
		lpRmcInfo->chLatNS = f_szTemp2[1];
		TRUEReturn(lpRmcInfo->chLatNS != 'N' &&  lpRmcInfo->chLatNS != 'S',CUNPACK_NULL);
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		FALSEReturn(CheckDouble(f_szTemp2+1),CUNPACK_NULL);
		FALSEReturn(IntpartLen(f_szTemp2+1)==5 ,CUNPACK_NULL);

		strInfo = f_szTemp2 +1;
		FALSEReturn(strInfo.size() > 2,CUNPACK_NULL);
		lpRmcInfo->dfLong  = atoi(strInfo.substr(0,3).c_str());
		const char * pstr  = strInfo.c_str();
		double dfData = atof(pstr+3);
		lpRmcInfo->dfLong += dfData/60.0;
		FALSEReturn(CheckLCloseRange(dfData,0,60),CUNPACK_NULL);
		lpRmcInfo->dfLong += dfData/60.0;

		BOOL bCheck = CheckLCloseRange(lpRmcInfo->dfLong,0,180);
		FALSEReturn(bCheck,CUNPACK_NULL);
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		FALSEReturn(FieldLength(f_szTemp2+1) == 1,CUNPACK_NULL);
		lpRmcInfo->chLongEW = f_szTemp2[1];
		TRUEReturn(lpRmcInfo->chLongEW != 'E' && lpRmcInfo->chLongEW != 'W',CUNPACK_NULL);
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		lpRmcInfo->fGrndVel = atof(f_szTemp2+1);
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		lpRmcInfo->fGrndDir = atof(f_szTemp2+1);

		bCheck = CheckLCloseRange(lpRmcInfo->fGrndDir,0,360);
		BOOL bCheck1 = CheckRCloseRange(lpRmcInfo->fGrndDir,-360,0);

		FALSEReturn(bCheck||bCheck1,CUNPACK_NULL);
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);			

	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		int  nFieldLen = FieldLength(f_szTemp2+1);
		bCheck = (nFieldLen==6);
		FALSEReturn(bCheck,CUNPACK_NULL);

		if(CheckInt(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;

		strInfo = f_szTemp2 +1;
		lpRmcInfo->nDay   = atoi(strInfo.substr(0,2).c_str());
		lpRmcInfo->nMonth = atoi(strInfo.substr(2,2).c_str());
		lpRmcInfo->nYear  = atoi(strInfo.substr(4,2).c_str());

		FALSEReturn(lpRmcInfo->nDay<32,CUNPACK_NULL);
		FALSEReturn(lpRmcInfo->nMonth<13,CUNPACK_NULL);
		FALSEReturn(lpRmcInfo->nYear <51,CUNPACK_NULL);
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2 + 1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		lpRmcInfo->fMagAngle = atof(f_szTemp2 + 1);
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		lpRmcInfo->chMagDir = f_szTemp2[1];
		TRUEReturn(lpRmcInfo->chMagDir != 'E' && lpRmcInfo->chMagDir != 'W',CUNPACK_NULL);
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ','&&f_szTemp2[1] != '*')
	{
		lpRmcInfo->chModeFlag = f_szTemp2[1];
		TRUEReturn(lpRmcInfo->chModeFlag != 'A' && lpRmcInfo->chModeFlag != 'D'&&lpRmcInfo->chModeFlag != 'E'&&lpRmcInfo->chModeFlag!='N',CUNPACK_NULL);

	}

	Len = sizeof(RMCInfo);

	return CUNPACK_RMC;
}

int CRdss2_1SearchExp::explain_gga(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	string strInfo;
	LPGGAInfo lpGGAInfo = (LPGGAInfo)DataBuff;
	memset(lpGGAInfo,0,sizeof(GGAInfo));

	FRAME_SYS((char*)byFrame,lpGGAInfo->chSys,sizeof(lpGGAInfo->chSys));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);

	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck   = FALSE;
		int nFieldLen = FieldLength(f_szTemp2+1);//
		bCheck = (nFieldLen > 5);
		FALSEReturn(bCheck,CUNPACK_NULL);

		bCheck = CheckDouble(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL) ;

		if(nFieldLen ==6)
			FALSEReturn(CheckInt(f_szTemp2+1),CUNPACK_NULL);

		strInfo = f_szTemp2 + 1;
		FALSEReturn(strInfo.size() > 3,CUNPACK_NULL);
		lpGGAInfo->nPosHour   = atoi(strInfo.substr(0,2).c_str());
		lpGGAInfo->nPosMinute = atoi(strInfo.substr(2,2).c_str());
		const char * pstr     = strInfo.c_str();
		lpGGAInfo->fPosSecond = atof(pstr+4);

		bCheck = CheckCloseRange(lpGGAInfo->nPosHour,0,23);
		FALSEReturn(bCheck,CUNPACK_NULL) ;

		bCheck = CheckCloseRange(lpGGAInfo->nPosMinute,0,59);
		FALSEReturn(bCheck,CUNPACK_NULL) ;

		bCheck = CheckLCloseRange(lpGGAInfo->fPosSecond,0,60);
		FALSEReturn(bCheck,CUNPACK_NULL) ;
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] !=',')
	{

		FALSEReturn(CheckDouble(f_szTemp2+1),CUNPACK_NULL);
		FALSEReturn(IntpartLen(f_szTemp2+1)==4 ,CUNPACK_NULL);

		strInfo = f_szTemp2+1;
		FALSEReturn(strInfo.size() > 1,CUNPACK_NULL);

		lpGGAInfo->dfLat =  atoi(strInfo.substr(0,2).c_str());
		const char * pstr = strInfo.c_str();
		double dfData = atof(pstr+2);
		FALSEReturn(CheckLCloseRange(dfData,0,60),CUNPACK_NULL);
		lpGGAInfo->dfLat += dfData/60.0;
		BOOL bCheck = CheckCloseRange(lpGGAInfo->dfLat,0,90);
		FALSEReturn(bCheck,CUNPACK_NULL);
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		FALSEReturn(f_szTemp2[1]=='N'||f_szTemp2[1]=='S',CUNPACK_NULL);
		lpGGAInfo->chLatNS = f_szTemp2[1];
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);			

	if(f_szTemp2[1] != ',')
	{
		FALSEReturn(CheckDouble(f_szTemp2+1),CUNPACK_NULL);
		FALSEReturn(IntpartLen(f_szTemp2+1)==5 ,CUNPACK_NULL);

		strInfo = f_szTemp2+1;
		FALSEReturn(strInfo.size() > 2,CUNPACK_NULL);
		lpGGAInfo->dfLong =  atoi(strInfo.substr(0,3).c_str());
		const char* pstr  =  strInfo.c_str();
		double dfData = atof(pstr+3);
		FALSEReturn(CheckLCloseRange(dfData,0,60),CUNPACK_NULL);
		lpGGAInfo->dfLong += dfData/60.0;
		BOOL bCheck = CheckLCloseRange(lpGGAInfo->dfLong,0,180);
		FALSEReturn(bCheck,CUNPACK_NULL);
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1]!=',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		FALSEReturn(f_szTemp2[1]=='E'||f_szTemp2[1]=='W',CUNPACK_NULL);
		lpGGAInfo->chLongEW = f_szTemp2[1];
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		TRUEReturn(f_szTemp2[1] < '0' || f_szTemp2[1] > '8',CUNPACK_NULL);
		lpGGAInfo->byStatus = atoi(&f_szTemp2[1]);
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2 +1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		lpGGAInfo->bySatNum = atoi(f_szTemp2 +1);
	}

	if(lpGGAInfo->bySatNum > 12)
		return CUNPACK_NULL;

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] !=',')
	{
		if(CheckDouble(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;
		lpGGAInfo->fHdop = atof(f_szTemp2 +1);
	}
	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] !=',')
	{
		if(CheckDouble(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;
		lpGGAInfo->dfGndHigh = atof(f_szTemp2 +1);
	}
	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);			

	if(f_szTemp2[1] !=',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);
		lpGGAInfo->chGndHighUint = f_szTemp2[1];
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] !=',')
	{
		if(CheckDouble(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;
		lpGGAInfo->dfHgtExcept = atof(f_szTemp2 +1);
	}
	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] !=',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		lpGGAInfo->chHgtExceptUint = f_szTemp2[1];
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] !=',')
	{
		if(CheckInt(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;
		lpGGAInfo->DiffDate = atoi(f_szTemp2+1);
	}
	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);			

	if(f_szTemp2[1] !=',')
	{
		f_szTemp2 += 1;
		int c;
		for( c = 0 ; c < sizeof(lpGGAInfo->chDiffStation)-1 ;c++)
		{
			if(f_szTemp2[c] == ','||f_szTemp2[c] == '\0')
				break;
			else
				lpGGAInfo->chDiffStation[c] = f_szTemp2[c];
		}
		lpGGAInfo->chDiffStation[c] = '\0';
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != '*')
	{
		if(CheckDouble(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;
		lpGGAInfo->dfVdop = atof(f_szTemp2 +1);
	}

	Len = sizeof(GGAInfo);

	return CUNPACK_GGA;
}

int CRdss2_1SearchExp::explain_bsi(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	tagBeamInfo* pBeamInfo =  (tagBeamInfo*)DataBuff ;
	memset(pBeamInfo,0,sizeof(tagBeamInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		if(CheckInt(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;
		pBeamInfo->byResponseNo = atoi(&f_szTemp2[1]);
		if(!CheckCloseRange(pBeamInfo->byResponseNo,1,10))
			return CUNPACK_NULL;
	}
	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		if(CheckInt(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;
		pBeamInfo->byTimeDiffNo = atoi(&f_szTemp2[1]);
		if(!CheckCloseRange(pBeamInfo->byTimeDiffNo,0,10))
			return CUNPACK_NULL;
	}			

	for(int i =0 ; i< 5 ;i++)
	{
		f_szTemp2 = strstr(&f_szTemp2[1], ",");
		NULLReturn(f_szTemp2,CUNPACK_NULL);
		BOOL bCheck = CheckDouble(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		if(f_szTemp2[1] != ',')
		{
			pBeamInfo->byPower[i*2] = atoi(&f_szTemp2[1]);
			if(!CheckCloseRange(pBeamInfo->byPower[i*2],0,4))
				return CUNPACK_NULL;
		}

		f_szTemp2 = strstr(&f_szTemp2[1], ",");
		NULLReturn(f_szTemp2,CUNPACK_NULL);

		bCheck = CheckDouble(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		if(f_szTemp2[1] != ',' && f_szTemp2[1] !='*')
		{
			pBeamInfo->byPower[i*2+1] = atoi(&f_szTemp2[1]);

			if(!CheckCloseRange(pBeamInfo->byPower[i*2+1],0,4))
				return CUNPACK_NULL;
		}
	}
	Len = sizeof(tagBeamInfo);
	return CUNPACK_BSI;
}

int CRdss2_1SearchExp::explain_dwr(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	double dfData;
	int    nTemp ;
	string strTemp;
	char * pstr = NULL;
	string strData;

	lpDWRInfo pDWRInfo = (lpDWRInfo)DataBuff;
	memset(pDWRInfo,0,sizeof(DWRInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pDWRInfo->nPosType = atoi(f_szTemp2+1);
		if(!CheckCloseRange(pDWRInfo->nPosType,1,3))
			return CUNPACK_NULL;
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL  bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pDWRInfo->nLocalId = atoi(&f_szTemp2[1]);
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{

		BOOL bCheck = FALSE;
		int  nFieldLen = FieldLength(f_szTemp2+1);//
		bCheck = (nFieldLen > 5);
		FALSEReturn(bCheck,CUNPACK_NULL);

		FALSEReturn(CheckDouble(f_szTemp2+1),CUNPACK_NULL);

		if(nFieldLen == 6)
			FALSEReturn(CheckInt(f_szTemp2+1),CUNPACK_NULL);

		strData = (f_szTemp2 + 1);
		FALSEReturn(strData.size() > 3,CUNPACK_NULL);

		pDWRInfo->nPosHour   = atoi(strData.substr(0,2).c_str());
		pDWRInfo->nPosHour = (pDWRInfo->nPosHour+8)%24;
		bCheck = CheckCloseRange(pDWRInfo->nPosHour,0,23);
		FALSEReturn(bCheck,CUNPACK_NULL);

		pDWRInfo->nPosMinute = atoi(strData.substr(2,2).c_str());
		bCheck = CheckCloseRange(pDWRInfo->nPosMinute,0,59);
		FALSEReturn(bCheck,CUNPACK_NULL);

		const char* pstr1 = strData.c_str();
		pDWRInfo->fPosSecond = atof(pstr1+4);
		bCheck = CheckLCloseRange(pDWRInfo->fPosSecond,0,60);
		FALSEReturn(bCheck,CUNPACK_NULL);
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);			

	if(f_szTemp2[1] != ',')
	{
		strData = f_szTemp2+1;
		BOOL bCheck = FALSE;

		FALSEReturn(CheckDouble(f_szTemp2+1),CUNPACK_NULL);
		FALSEReturn(IntpartLen(f_szTemp2+1) == 4,CUNPACK_NULL);
		FALSEReturn(strData.size()>1,CUNPACK_NULL);

		strTemp = strData.substr(0,2) ;
		nTemp  = atoi(strTemp.c_str());

		const char* pstr1 = strData.c_str();

		dfData = atof(pstr1+2);
		bCheck = CheckLCloseRange(dfData,0,60);
		FALSEReturn(bCheck,CUNPACK_NULL);
		dfData = dfData /60 + nTemp;
		pDWRInfo->dfLat = dfData;
		bCheck = CheckCloseRange(pDWRInfo->dfLat,0,90);
		FALSEReturn(bCheck,CUNPACK_NULL);
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{//caution
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		TRUEReturn(f_szTemp2[1] != 'N' && f_szTemp2[1] != 'S',CUNPACK_NULL);
		pDWRInfo->chLatDirection = f_szTemp2[1];
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		strData = f_szTemp2+1;
		BOOL bCheck = FALSE;

		FALSEReturn(CheckDouble(f_szTemp2+1) , CUNPACK_NULL);
		FALSEReturn(IntpartLen(f_szTemp2+1) == 5,CUNPACK_NULL);

		FALSEReturn(strData.size()>2,CUNPACK_NULL);
		nTemp  = atoi(strData.substr(0,3).c_str());
		const char* pstr1 = strData.c_str();
		dfData = atof(pstr1+3);
		bCheck = CheckLCloseRange(dfData,0,60);
		FALSEReturn(bCheck,CUNPACK_NULL);
		dfData = dfData /60 + nTemp;

		pDWRInfo->dfLong = dfData;
		bCheck = CheckLCloseRange(pDWRInfo->dfLong,0,180);//caution
		FALSEReturn(bCheck,CUNPACK_NULL);
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);			

	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		TRUEReturn(f_szTemp2[1] != 'W' && f_szTemp2[1] != 'E',CUNPACK_NULL);

		pDWRInfo->chLongDirection = f_szTemp2[1];
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{//caution
		if(CheckDouble(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;
		pDWRInfo->fEarthHgt = atof(&f_szTemp2[1]);
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		pDWRInfo->chHgtUnit = f_szTemp2[1];
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);//caution
	if(f_szTemp2[1] != ',')
	{
		if(CheckDouble(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;
		pDWRInfo->dfHgtOut = atof(&f_szTemp2[1]);
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL) ;
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		pDWRInfo->chHgtOutUnit = f_szTemp2[1];
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{//caution
		if(CheckInt(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;
		pDWRInfo->byPrecisionInd = atoi(&f_szTemp2[1]);

		if(!CheckCloseRange(pDWRInfo->byPrecisionInd,0,1))
			return CUNPACK_NULL;
	}
	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);			

	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		FALSEReturn(f_szTemp2[1] == 'A' || f_szTemp2[1] == 'V',CUNPACK_NULL);
		if(f_szTemp2[1] == 'A')
			pDWRInfo->bUrgent = TRUE;//caution
		else
			pDWRInfo->bUrgent = FALSE;
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		FALSEReturn(f_szTemp2[1] == 'A' || f_szTemp2[1] == 'V',CUNPACK_NULL);
		if(f_szTemp2[1] == 'A')
			pDWRInfo->bMultiValue = TRUE;
		else
			pDWRInfo->bMultiValue = FALSE;
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ','&&f_szTemp2[1] != '*')
	{//caution
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		FALSEReturn(f_szTemp2[1] == 'H' || f_szTemp2[1] == 'L',CUNPACK_NULL);
		if(f_szTemp2[1] == 'H')
			pDWRInfo->bGaoKong = TRUE;
		else
			pDWRInfo->bGaoKong = FALSE;
	}

	Len = sizeof(DWRInfo);
	return CUNPACK_DWR;
}

int CRdss2_1SearchExp::explain_fki(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	DWORD dwData = 0;
	int   f_iCurLen = 0;
	string strData;

	//TRACE1("%s",CString(byFrame));

	lpFKIInfo pFKIInfo = (lpFKIInfo)DataBuff;
	memset(pFKIInfo,0,sizeof(FKIInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		int FieldLen = FieldLength(f_szTemp2+1);
		if(FieldLen != 3)
			return CUNPACK_NULL;//caution
		pFKIInfo->chCommandName[0] =(f_szTemp2[1]);
		pFKIInfo->chCommandName[1] =(f_szTemp2[2]);
		pFKIInfo->chCommandName[2] =(f_szTemp2[3]);//caution
	}			

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		FALSEReturn(f_szTemp2[1] == 'Y' || f_szTemp2[1] == 'N',CUNPACK_NULL);
		pFKIInfo->chExeRslt= f_szTemp2[1];
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);//caution
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		FALSEReturn(f_szTemp2[1] == 'Y' || f_szTemp2[1] == 'N',CUNPACK_NULL);
		pFKIInfo->chIntervalInd = f_szTemp2[1];
	}
	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pFKIInfo->byTranInhabitInd = atoi(&f_szTemp2[1]);


		if(!CheckCloseRange(pFKIInfo->byTranInhabitInd,0,3))
			return CUNPACK_NULL;
	}

	f_szTemp2 = strstr(&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);

	if(f_szTemp2[1] != '*' && f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		strData = (f_szTemp2 + 1);

		if(CheckInt(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;
		int FieldLen = FieldLength(f_szTemp2+1);//
		if(FieldLen != 4)
			return CUNPACK_NULL;

		pFKIInfo->byMinuteWait= atoi(strData.substr(0,2).c_str());
		bCheck = CheckCloseRange(pFKIInfo->byMinuteWait,0,59);
		FALSEReturn(bCheck,CUNPACK_NULL);

		pFKIInfo->bySecondWait= atoi(strData.substr(2,2).c_str());
		bCheck = CheckCloseRange(pFKIInfo->bySecondWait,0,59);
		FALSEReturn(bCheck,CUNPACK_NULL);
	}

	Len = sizeof(tagFKIInfo);

	return CUNPACK_FKI;
}

int CRdss2_1SearchExp::explain_ici(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	lpICIInfo pICIInfo = (lpICIInfo)DataBuff;
	memset(pICIInfo,0,sizeof(ICIInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);//caution
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pICIInfo->nLocalId = atoi(&f_szTemp2[1]);//caution
		m_uLocalID = pICIInfo->nLocalId ;
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pICIInfo->nSerialNo = atoi(&f_szTemp2[1]);//caution
	}

	f_szTemp2 = strstr((char*)&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pICIInfo->nBroadId = atoi(&f_szTemp2[1]);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pICIInfo->nUsrAttribute = atoi(&f_szTemp2[1]);
	}
	f_szTemp2 = strstr((char*)&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pICIInfo->nServiceFreq = atoi(&f_szTemp2[1]);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')//caution
	{
		if(CheckInt(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;
		pICIInfo->byCommLevel = atoi(&f_szTemp2[1]);

		if(!CheckCloseRange(pICIInfo->byCommLevel,1,4))
			return CUNPACK_NULL;
	}

	f_szTemp2 = strstr((char*)&f_szTemp2[1], ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);

	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		FALSEReturn(f_szTemp2[1] == 'E' || f_szTemp2[1] == 'N',CUNPACK_NULL);
		pICIInfo->chEncriptFlag = f_szTemp2[1];
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',' && f_szTemp2[1] != '*')
	{
		BOOL bCheck = CheckDouble(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pICIInfo->nSubUsersNum = atoi(&f_szTemp2[1]);
	}
	if(m_ici == NULL){
		m_ici = (tagICIInfo*)calloc(1,sizeof(tagICIInfo));
		memcpy(m_ici,pICIInfo,sizeof(tagICIInfo));
	}

	Len = sizeof(ICIInfo);
	return CUNPACK_ICI;
}

int CRdss2_1SearchExp::explain_icz(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	int nUserCnt = 0;
	lpICZInfo pICZInfo = (lpICZInfo)DataBuff;
	memset(pICZInfo,0,sizeof(ICZInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);//caution
		pICZInfo->nTotalSubUserNums = atoi(&f_szTemp2[1]);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	while(f_szTemp2 != NULL && f_szTemp2[1] != '*' && nUserCnt < 40)
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pICZInfo->nSubUserIds[nUserCnt++] = atoi(f_szTemp2+1);
		f_szTemp2 = strstr(f_szTemp2+1, ",");
	}

	pICZInfo->nUsrCntInFrm = nUserCnt;
	Len = sizeof(ICZInfo);

	return CUNPACK_ICZ;
}

int CRdss2_1SearchExp::explain_kls(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	lpKLSInfo pKLSInfo = (lpKLSInfo)DataBuff;
	f_szTemp2 = strstr((char*)byFrame, ",");

	NULLReturn(f_szTemp2,CUNPACK_NULL);
	pKLSInfo->nSubUsrId = atoi(&f_szTemp2[1]);

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	FALSEReturn(f_szTemp2[1] != '*'&& f_szTemp2[1] != ',',CUNPACK_NULL);
	pKLSInfo->chResponseFlag = f_szTemp2[1];

	Len = sizeof(tagKLSInfo);

	return CUNPACK_KLS;
}

int CRdss2_1SearchExp::explain_hzr(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	string strTime;

	lpHZRInfo pHZRInfo = (lpHZRInfo)DataBuff;
	memset(pHZRInfo,0,sizeof(HZRInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);//caution
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pHZRInfo->nLocalId = atoi(&f_szTemp2[1]);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pHZRInfo->byResponseNum = atoi(f_szTemp2+1);

		if(!CheckCloseRange(pHZRInfo->byResponseNum,0,5))
			return CUNPACK_NULL;

	}

	for(int i = 0 ; i < pHZRInfo->byResponseNum ; i++)
	{//caution
		f_szTemp2 = strstr(f_szTemp2+1, ",");
		NULLReturn(f_szTemp2,CUNPACK_NULL);				

		if(f_szTemp2[1] != ',')
		{
			strTime = f_szTemp2+1;

			int FieldLen = FieldLength(f_szTemp2+1);//
			if(FieldLen != 4)
				return CUNPACK_NULL;
			if(CheckInt(f_szTemp2+1) == FALSE)
				return CUNPACK_NULL;

			pHZRInfo->wSendHour  [i]   = atoi(strTime.substr(0,2).c_str());
			pHZRInfo->wSendHour  [i] =  (pHZRInfo->wSendHour  [i]+8)%24;
			pHZRInfo->wSendMinute[i]   = atoi(strTime.substr(2,2).c_str());//caution

			if(!CheckCloseRange(pHZRInfo->wSendHour  [i],0,23))
				return CUNPACK_NULL;
			if(!CheckCloseRange(pHZRInfo->wSendMinute  [i],0,59))
				return CUNPACK_NULL;
		}//caution

		f_szTemp2 = strstr(f_szTemp2+1, ",");
		NULLReturn(f_szTemp2,CUNPACK_NULL);
		if(f_szTemp2[1] != ','&& (f_szTemp2[1]!='*'))
		{
			strTime = f_szTemp2+1;

			int FieldLen = FieldLength(f_szTemp2+1);//
			if(FieldLen != 4)
				return CUNPACK_NULL;
			if(CheckInt(f_szTemp2+1) == FALSE)
				return CUNPACK_NULL;

			pHZRInfo->wAckHour  [i]   = atoi(strTime.substr(0,2).c_str());
			pHZRInfo->wAckHour  [i] = (pHZRInfo->wAckHour[i] +8)%24;
			pHZRInfo->wAckMinute[i]   = atoi(strTime.substr(2,2).c_str());

			if(!CheckCloseRange(pHZRInfo->wAckHour  [i],0,23))
				return CUNPACK_NULL;
			if(CheckCloseRange(pHZRInfo->wAckMinute  [i],0,59)!= TRUE)
				return CUNPACK_NULL;
		}
	}

	Len = sizeof(HZRInfo);
	return CUNPACK_HZR;
}

int CRdss2_1SearchExp::explain_txr(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	string strTime;
	DWORD  nBitLen = 0;

	lpTXRInfo pTXRInfo = (lpTXRInfo)DataBuff;
	memset(pTXRInfo,0,sizeof(TXRInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");

	NULLReturn(f_szTemp2,CUNPACK_NULL);			

	if(f_szTemp2[1]!= ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pTXRInfo->byInfoType = atoi(&f_szTemp2[1]);
		if(CheckCloseRange(pTXRInfo->byInfoType,1,5)!= TRUE)//caution
			return CUNPACK_NULL;
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1]!= ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pTXRInfo->nLocalID = atoi(&f_szTemp2[1]);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1]!= ',')
	{//caution
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pTXRInfo->byTeleFormat = atoi(&f_szTemp2[1]);
		if(CheckCloseRange(pTXRInfo->byTeleFormat,0,2)!= TRUE)
			return CUNPACK_NULL;
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1]!= ',')
	{
		BOOL bCheck = FALSE;
		strTime = f_szTemp2+1;
		if(CheckInt(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;

		int FieldLen = FieldLength(f_szTemp2+1);//

		if(FieldLen == 4)
		{
			pTXRInfo->bySendHour  = atoi(strTime.substr(0,2).c_str());
			bCheck = CheckCloseRange(pTXRInfo->bySendHour,0,23);
			FALSEReturn(bCheck,CUNPACK_NULL);//caution

			pTXRInfo->bySendMinute= atoi(strTime.substr(2,2).c_str());
			bCheck = CheckCloseRange(pTXRInfo->bySendMinute,0,59);
			FALSEReturn(bCheck,CUNPACK_NULL);
		}else
		{
			pTXRInfo->bySendHour  = 0;
			pTXRInfo->bySendMinute= 0;
		}
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		f_szTemp2++;

		int i = 0 ;
		for(i = 0 ;i< sizeof(pTXRInfo->chContent)-1; i++)
		{
			if(f_szTemp2[i-4] == '*'&&f_szTemp2[i-1] == '\r' &&f_szTemp2[i] == '\n' )
				break;
		}

		memcpy(pTXRInfo->chContent,f_szTemp2,i-4);
		pTXRInfo->chContent[i-4] = '\0';
		pTXRInfo->nContentLen    = i-4 ;
	}			

	if(pTXRInfo->byTeleFormat == 1)
	{
		int j = 0 ;
		for(; j <pTXRInfo->nContentLen; j++)
		{
			if(!IsHex(pTXRInfo->chContent[j]))
				break;
		}

		TRUEReturn(j < pTXRInfo->nContentLen,CUNPACK_NULL);
	}

	if(pTXRInfo->byTeleFormat == 2)
	{
		TRUEReturn(pTXRInfo->nContentLen <2 ,CUNPACK_NULL);
		if(pTXRInfo->chContent[0] != 'A')
			return CUNPACK_NULL;

		char chSet[4] = {'0','2','3','4'};
		BOOL bCheck   = CheckInSet(pTXRInfo->chContent[1],chSet,4);
		if(!bCheck)
			return CUNPACK_NULL;

		char buffer[512];

		int bits = hexchars_bytes((unsigned char*)buffer,sizeof(buffer),(char *)(pTXRInfo->chContent+2),pTXRInfo->nContentLen-2);
		memcpy_s((unsigned char*)pTXRInfo->chContent,sizeof(pTXRInfo->chContent),(unsigned char*)buffer,bits/8+(bits%8?1:0));
		pTXRInfo->nContentLen = bits/8+(bits%8?1:0);
		pTXRInfo->chContent[pTXRInfo->nContentLen] = '\0';
	}
	pTXRInfo->nDestionID = m_uLocalID;

	Len = sizeof(TXRInfo);
	return CUNPACK_TXR;
}

int CRdss2_1SearchExp::explain_lzm(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	LPLZMInfo pLZMInfo = (LPLZMInfo)DataBuff;
	memset(pLZMInfo,0,sizeof(LZMInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
		pLZMInfo->byManageMode = atoi(f_szTemp2+1);

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ','&&f_szTemp2[1] != '*')
		pLZMInfo->dfZeroValue = atof(f_szTemp2+1);

	Len = sizeof(LZMInfo);

	return CUNPACK_LZM;
}

int CRdss2_1SearchExp::explain_waa(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	char* f_szTemp1 = NULL;
	f_szTemp2 = NULL;
	string  strTemp ;
	double  dfData = 0.0;
	lpWAAInfo pWAAInfo = (lpWAAInfo)DataBuff;
	memset(pWAAInfo,0,sizeof(WAAInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		if(CheckInt(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;//caution
		pWAAInfo->byInfoType= atoi(&f_szTemp2[1]);
		if(CheckCloseRange(pWAAInfo->byInfoType,0,1)!= TRUE)
			return CUNPACK_NULL;
	}

	f_szTemp2 = strstr((char*)f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		if(CheckDouble(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;
		pWAAInfo->nReportFreq = atof(&f_szTemp2[1]);
	}
	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pWAAInfo->nUserId = atoi(&f_szTemp2[1]);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{

		BOOL bCheck = FALSE;
		int  nFieldLen = FieldLength(f_szTemp2+1);//
		bCheck = (nFieldLen > 5);
		FALSEReturn(bCheck,CUNPACK_NULL);

		if(CheckDouble(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;

		if(nFieldLen == 6)
			FALSEReturn(CheckInt(f_szTemp2+1),CUNPACK_NULL);

		strTemp = f_szTemp2+1;//caution
		pWAAInfo->wReportHour = atoi(strTemp.substr(0,2).c_str());
		pWAAInfo->wReportHour = (pWAAInfo->wReportHour +8)%24;
		pWAAInfo->wReportMinute= atoi(strTemp.substr(2,2).c_str());
		const char * pstr = strTemp.c_str();
		pWAAInfo->fReportSecond= atof(pstr+4);

		bCheck = CheckCloseRange(pWAAInfo->wReportHour,0,23);
		FALSEReturn(bCheck,CUNPACK_NULL);

		bCheck = CheckCloseRange(pWAAInfo->wReportMinute,0,59);
		FALSEReturn(bCheck,CUNPACK_NULL);

		bCheck = CheckLCloseRange(pWAAInfo->fReportSecond,0,60);
		FALSEReturn(bCheck,CUNPACK_NULL);

	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		FALSEReturn(IntpartLen(f_szTemp2+1) == 4,CUNPACK_NULL);

		strTemp = f_szTemp2+1;

		pWAAInfo->dfLat = atoi(strTemp.substr(0,2).c_str());
		const char* pstr = strTemp.c_str();
		dfData = atof(pstr + 2);
		FALSEReturn(CheckLCloseRange(dfData,0,60),CUNPACK_NULL);//caution
		pWAAInfo->dfLat += dfData/60;

		bCheck = CheckCloseRange(pWAAInfo->dfLat,0,90);
		FALSEReturn(bCheck,CUNPACK_NULL);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);//caution
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);
		FALSEReturn(f_szTemp2[1] == 'N' || f_szTemp2[1] == 'S',CUNPACK_NULL);
		pWAAInfo->chLatDir = *(f_szTemp2+1);
	}
	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{//
		BOOL bCheck = CheckDouble(f_szTemp2+1);//
		FALSEReturn(bCheck,CUNPACK_NULL);//
		FALSEReturn(IntpartLen(f_szTemp2+1) == 5,CUNPACK_NULL);//

		strTemp = f_szTemp2+1;//11420.736
		pWAAInfo->dfLong = atoi(strTemp.substr(0,3).c_str());//
		const char * pstr = strTemp.c_str();//caution
		dfData = atof(pstr + 3);//
		FALSEReturn(CheckLCloseRange(dfData,0,60),CUNPACK_NULL);//
		pWAAInfo->dfLong += dfData/60;//
		//
		bCheck = CheckLCloseRange(pWAAInfo->dfLong,0,180);//
		FALSEReturn(bCheck,CUNPACK_NULL);//
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);
		FALSEReturn(f_szTemp2[1] == 'W' || f_szTemp2[1] == 'E',CUNPACK_NULL);
		pWAAInfo->chLongDir = (f_szTemp2[1]);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		if(CheckDouble(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;
		pWAAInfo->dfHgt = atof(f_szTemp2+1);
	}
	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);//caution

	if(f_szTemp2[1] != ','&&f_szTemp2[1] != '*')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		pWAAInfo->chHgtUnit = (f_szTemp2[1]);
	}

	Len = sizeof(WAAInfo);

	return CUNPACK_WAA;
}

int CRdss2_1SearchExp::explain_sbx(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	char *f_szTemp1 = NULL;
	lpSBXInfo pSBXInfo = (lpSBXInfo)DataBuff;
	memset(pSBXInfo,0,sizeof(SBXInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	f_szTemp2 ++;
	int i;
	for(i = 0 ; i< sizeof(pSBXInfo->chManufac)-1; i++)
	{
		if(f_szTemp2[i] == ',')
			break;

		pSBXInfo->chManufac[i] = f_szTemp2[i];
	}//caution

	f_szTemp1 = strstr(f_szTemp2, ",");
	NULLReturn(f_szTemp1,CUNPACK_NULL);
	if(f_szTemp1[1] != ',')
	{
		f_szTemp1 ++;
		for(i = 0 ; i< sizeof(pSBXInfo->chDevType)-1; i++)
		{
			if(f_szTemp1[i] == ',')
				break;

			pSBXInfo->chDevType[i] = f_szTemp1[i];
		}
	}

	f_szTemp2 = strstr((char*)f_szTemp1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);			

	if(f_szTemp2[1] != ',')
	{
		f_szTemp2 ++;
		for( i = 0 ; i< sizeof(pSBXInfo->chSoftVer)-1; i++)
		{
			if(f_szTemp2[i] == ',')
				break;

			pSBXInfo->chSoftVer[i] = f_szTemp2[i];
		}

	}

	f_szTemp1 = strstr(f_szTemp2, ",");
	NULLReturn(f_szTemp1,CUNPACK_NULL);
	if(f_szTemp1[1] != ',')
	{
		f_szTemp1 ++;
		for( i = 0 ; i< sizeof(pSBXInfo->chAppProtocalVer)-1; i++)
		{
			if(f_szTemp1[i] == ',')
				break;//caution

			pSBXInfo->chAppProtocalVer[i] = f_szTemp1[i];
		}
	}

	f_szTemp2 = strstr((char*)f_szTemp1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);//caution
	if(f_szTemp2[1] != ',')
	{
		f_szTemp2 ++;
		for( i = 0 ; i< sizeof(pSBXInfo->chICDVer)-1; i++)
		{
			if(f_szTemp2[i] == ',')
				break;

			pSBXInfo->chICDVer[i] = f_szTemp2[i];
		}
	}

	f_szTemp1 = strstr(f_szTemp2, ",");
	NULLReturn(f_szTemp1,CUNPACK_NULL);			

	if(f_szTemp1[1] != ',')
	{
		f_szTemp1 ++;
		for( i = 0 ; i< sizeof(pSBXInfo->chDevSerialNo)-1; i++)
		{
			if(f_szTemp1[i] == ',')
				break;

			pSBXInfo->chDevSerialNo[i] = f_szTemp1[i];
		}
	}

	f_szTemp2 = strstr((char*)f_szTemp1, "*");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ','&&f_szTemp2[1] != '*')
	{
		if(CheckInt(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;
		f_szTemp2 ++;
		pSBXInfo->nUserId = atoi(f_szTemp2);
	}

	Len = sizeof(SBXInfo);

	return CUNPACK_SBX;
}

int CRdss2_1SearchExp::explain_zti(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	char   chData = '0';
	string strInfo;
	//caution
	lpZTIInfo pZTIInfo = (lpZTIInfo)DataBuff;
	memset(pZTIInfo,0,sizeof(ZTIInfo));
	pZTIInfo->type=0;
	pZTIInfo->data.zti_t.byStatusMask = 0;

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);//caution

		chData =  f_szTemp2[1];
		FALSEReturn(chData == '0' || chData == '1', CUNPACK_NULL);//caution
		BYTEBitAsign(&(pZTIInfo->data.zti_t.byStatusMask),MASK_PRMMODULE,(chData == '1'));//caution
	}


	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);////caution
		FALSEReturn(bCheck,CUNPACK_NULL);

		chData =  f_szTemp2[1];
		TRUEReturn(chData != '0' && chData != '1',CUNPACK_NULL);
		BYTEBitAsign(&(pZTIInfo->data.zti_t.byStatusMask),MASK_ENCRYPMODULE,(chData == '1'));
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//modf liu
		FALSEReturn(bCheck,CUNPACK_NULL);//modf liu

		chData =  f_szTemp2[1];//modf liu
		TRUEReturn(chData != '0' && chData != '1',CUNPACK_NULL);//modf liu
		BYTEBitAsign(&(pZTIInfo->data.zti_t.byStatusMask),MASK_ANTENNA,(chData == '1'));
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);////caution
		FALSEReturn(bCheck,CUNPACK_NULL);

		chData =  f_szTemp2[1];//modf liu
		TRUEReturn(chData != '0' && chData != '1',CUNPACK_NULL);//modf liu
		BYTEBitAsign(&(pZTIInfo->data.zti_t.byStatusMask),MASK_CHANNEL,(chData == '1'));
	}


	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{//modf liu
		BOOL bCheck = FALSE;//modf liu
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);//modf liu
		//modf liu
		chData =  f_szTemp2[1];//modf liu
		TRUEReturn(chData != '0' && chData != '1',CUNPACK_NULL);//modf liu
		BYTEBitAsign(&(pZTIInfo->data.zti_t.byStatusMask),MASK_POWER_INOUT,(chData == '1'));//modf liu
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");//modf liu
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{//modf liu
		BOOL bCheck = FALSE;//modf liu
		bCheck = (FieldLength(f_szTemp2+1) == 1);//caution
		FALSEReturn(bCheck,CUNPACK_NULL);//modf liu

		chData =  f_szTemp2[1];//modf liu
		TRUEReturn(chData != '0' && chData != '1',CUNPACK_NULL);//modf liu
		BYTEBitAsign(&(pZTIInfo->data.zti_t.byStatusMask),MASK_CHARGE,(chData == '1'));
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		if(CheckDouble(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;
		pZTIInfo->data.zti_t.fPowerRemainPercent = atof(f_szTemp2 +1);
	}
	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ','&&f_szTemp2[1] != '*')//modf liu//modf liu
	{
		strInfo = f_szTemp2 +1;
		int FieldLen = FieldLength(f_szTemp2+1);//
		if(FieldLen != 4)//modf liu
			return CUNPACK_NULL;
		if(CheckInt(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;
		//modf liu
		pZTIInfo->data.zti_t.byPowerRemainMinute = atoi(strInfo.substr(0,2).c_str())*60 + atoi(strInfo.substr(2,2).c_str());//modf liu
	}

	Len = sizeof(ZTIInfo);

	return CUNPACK_ZTI;
}

int CRdss2_1SearchExp::explain_zda(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	string strTime ;
	f_szTemp2 = 0  ;
	lpZDAInfo pZDAInfo = (lpZDAInfo)DataBuff;
	memset(pZDAInfo,0,sizeof(ZDAInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		if(CheckDouble(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;
		//modf liu
		pZDAInfo->nMode = ((BYTE)f_szTemp2[1] - 0x30);
		//modf liu
		if (CheckCloseRange(pZDAInfo->nMode,1,9) != TRUE)
			return CUNPACK_NULL;//modf liu
	}
	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		strTime = (f_szTemp2+1);

		BOOL bCheck = FALSE;
		int  nFieldLen = FieldLength(f_szTemp2+1);////modf liu
		bCheck = (nFieldLen > 5 );
		FALSEReturn(bCheck,CUNPACK_NULL);

		if(CheckDouble(f_szTemp2+1) == FALSE)//modf liu
			return CUNPACK_NULL;

		if(nFieldLen == 6)
			FALSEReturn(CheckInt(f_szTemp2+1),CUNPACK_NULL);//modf liu

		FALSEReturn(strTime.size()>3,CUNPACK_NULL);//modf liu
		pZDAInfo->wHour    = atoi(strTime.substr(0,2).c_str());//modf liu
		pZDAInfo->wHour = (pZDAInfo->wHour +8)%24;
		pZDAInfo->wMinute  = atoi(strTime.substr(2,2).c_str());//modf liu
		const char * pstr  = strTime.c_str();
		pZDAInfo->fSeconds = atof(pstr+4);
		bCheck = CheckCloseRange(pZDAInfo->wHour,0,23);
		FALSEReturn(bCheck,CUNPACK_NULL);

		bCheck = CheckCloseRange(pZDAInfo->wMinute,0,59);
		FALSEReturn(bCheck,CUNPACK_NULL);

		bCheck = CheckLCloseRange(pZDAInfo->fSeconds,0,60);
		FALSEReturn(bCheck,CUNPACK_NULL);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");//modf liu
	NULLReturn(f_szTemp2,CUNPACK_NULL);//modf liu
	if(f_szTemp2[1] != ',')//modf liu
	{//modf liu
		BOOL bCheck = CheckInt(f_szTemp2+1);//modf liu
		FALSEReturn(bCheck,CUNPACK_NULL);//modf liu
		pZDAInfo->wDay = atoi(f_szTemp2+1);//modf liu
		if(!CheckCloseRange(pZDAInfo->wDay,1,31))//modf liu
			return CUNPACK_NULL;
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')//modf liu
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);//modf liu
		FALSEReturn(bCheck,CUNPACK_NULL);
		pZDAInfo->wMonth = atoi(f_szTemp2+1);
		if(!CheckCloseRange(pZDAInfo->wMonth,1,12))
			return CUNPACK_NULL;
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");//modf liu
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pZDAInfo->wYear = atoi(f_szTemp2+1);
		if(pZDAInfo->wYear <2000)
			return CUNPACK_NULL;

	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");//modf liu
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pZDAInfo->nTimeArea = atoi(f_szTemp2+1);
		if(!CheckCloseRange(pZDAInfo->nTimeArea,-13,13))
			return CUNPACK_NULL;
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");//modf liu
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pZDAInfo->nLocalMinDiff = atoi(f_szTemp2+1);

		if(!CheckCloseRange(pZDAInfo->nLocalMinDiff,0,59))
			return CUNPACK_NULL;

	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		int  nFieldLen = FieldLength(f_szTemp2+1);////modf liu
		bCheck = (nFieldLen>5);
		FALSEReturn(bCheck,CUNPACK_NULL);

		if(CheckDouble(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;

		if(nFieldLen ==6)
			FALSEReturn(CheckInt(f_szTemp2+1),CUNPACK_NULL);

		strTime = f_szTemp2+1;
		FALSEReturn(strTime.size()>3,CUNPACK_NULL);
		//FALSEReturn(strTime.size() > 8,CUNPACK_NULL);
		pZDAInfo->wModifyHour    = atoi(strTime.substr(0,2).c_str());
		pZDAInfo->wModifyMinute  = atoi(strTime.substr(2,2).c_str());
		const char * pstr = strTime.c_str();
		pZDAInfo->fModifySeconds = atof(pstr+4);

		bCheck = CheckCloseRange(pZDAInfo->wModifyHour,0,59);//modf liu
		FALSEReturn(bCheck,CUNPACK_NULL);
		//modf liu
		bCheck = CheckCloseRange(pZDAInfo->wModifyMinute,0,59);//modf liu
		FALSEReturn(bCheck,CUNPACK_NULL);

		bCheck = CheckLCloseRange(pZDAInfo->fModifySeconds,0,60);//modf liu
		FALSEReturn(bCheck,CUNPACK_NULL);

	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pZDAInfo->dfValueModify = atof(f_szTemp2+1);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pZDAInfo->nPrecisionFlag = atoi(f_szTemp2+1);

		if(!CheckCloseRange(pZDAInfo->nPrecisionFlag,0,3))//modf liu
			return CUNPACK_NULL;
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ','&&f_szTemp2[1] != '*')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		FALSEReturn(f_szTemp2[1] == 'Y' || f_szTemp2[1] == 'N',CUNPACK_NULL);//modf liu
		pZDAInfo->chSatStatus = f_szTemp2[1];
	}
	Len = sizeof(ZDAInfo);

	return CUNPACK_ZDA;
}

int CRdss2_1SearchExp::explain_gsa(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	f_szTemp2  = NULL  ;
	int nSatCnt = 0;//
	lpGSAInfo pGSAInfo = (lpGSAInfo)DataBuff;
	memset(pGSAInfo,0,sizeof(GSAInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;///
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		FALSEReturn(f_szTemp2[1] == 'A' || f_szTemp2[1] == 'M',CUNPACK_NULL);//modf liu
		pGSAInfo->chMode = f_szTemp2[1];
	}
	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')//modf liu
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pGSAInfo->nModeOption = atoi(&f_szTemp2[1]);
		if(!CheckCloseRange(pGSAInfo->nModeOption,1,3))//modf liu
			return CUNPACK_NULL;
	}
	for(int i = 0 ; i < 12 ; i++)
	{///
		f_szTemp2 = strstr(f_szTemp2+1, ",");
		NULLReturn(f_szTemp2,CUNPACK_NULL);
		if(f_szTemp2[1] == ',')
			continue;

		int FieldLen = FieldLength(f_szTemp2+1);////modf liu
		if (FieldLen != 2)
			return CUNPACK_NULL;

		BOOL bCheck = CheckInt(f_szTemp2+1);//modf liu
		FALSEReturn(bCheck,CUNPACK_NULL);
		pGSAInfo->nSatNo[nSatCnt++] = atoi(f_szTemp2+1);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		if(CheckDouble(f_szTemp2+1) == FALSE)
			return CUNPACK_NULL;
		pGSAInfo->fPdop = atof(f_szTemp2+1);
	}
	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);//modf liu
		pGSAInfo->fHdop = atof(f_szTemp2+1);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);//modf liu
		pGSAInfo->fVdop = atof(f_szTemp2+1);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ','&&f_szTemp2[1] != '*')//modf liu
	{//modf liu
		BOOL bCheck = CheckDouble(f_szTemp2+1);//modf liu
		FALSEReturn(bCheck,CUNPACK_NULL);

		pGSAInfo->fTdop = atof(f_szTemp2+1);
	}

	Len = sizeof(GSAInfo);
	return CUNPACK_GSA;
}

int CRdss2_1SearchExp::explain_gxm(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	f_szTemp2 = NULL  ;
	int    nPos = 0;
	lpGXMInfo pGXMInfo = (lpGXMInfo)DataBuff;
	memset(pGXMInfo,0,sizeof(GXMInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");//modf liu
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{//modf liu//modf liu
		BOOL bCheck = CheckInt(f_szTemp2 + 1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pGXMInfo->nType = atoi(f_szTemp2 + 1);
	}

	FALSEReturn((pGXMInfo->nType == 3),CUNPACK_NULL);

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	f_szTemp2 ++;
	while((f_szTemp2[nPos]) != ','&& nPos < 64 && (f_szTemp2[nPos]) != '*')
	{
		FALSEReturn(IsHex(f_szTemp2[nPos]),CUNPACK_NULL);//modf liu
		pGXMInfo->chMagInfo[nPos] = f_szTemp2[nPos];
		nPos ++ ;
	}

	FALSEReturn(nPos==64,CUNPACK_NULL);			

	Len = sizeof(GXMInfo);

	return CUNPACK_GXM;
}

int CRdss2_1SearchExp::explain_dhv(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	f_szTemp2 = NULL;
	int    nPos = 0 ;
	lpDHVInfo pDHVInfo = (lpDHVInfo)DataBuff;
	memset(pDHVInfo,0,sizeof(DHVInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		int  nFieldLen = FieldLength(f_szTemp2+1);
		bCheck = (nFieldLen > 5);//modf liu
		FALSEReturn(bCheck,CUNPACK_NULL) ;//modf liu
		bCheck = CheckDouble(f_szTemp2+1);//modf liu
		FALSEReturn(bCheck,CUNPACK_NULL) ;
		if(nFieldLen == 6)
			FALSEReturn(CheckInt(f_szTemp2+1),CUNPACK_NULL);

		string strTime1(f_szTemp2 + 1);
		FALSEReturn(strTime1.size()>3,CUNPACK_NULL);//modf liu

		pDHVInfo->byHour   = atoi(strTime1.substr(0,2).c_str());//modf liu
		pDHVInfo->byMinute = atoi(strTime1.substr(2,2).c_str());
		const char * pstr  = strTime1.c_str();
		pDHVInfo->fSeconds = atof(pstr+4);

		bCheck = CheckCloseRange(pDHVInfo->byHour,0,59);//modf liu
		FALSEReturn(bCheck,CUNPACK_NULL);

		bCheck = CheckCloseRange(pDHVInfo->byMinute,0,59);//modf liu
		FALSEReturn(bCheck,CUNPACK_NULL);

		bCheck = CheckLCloseRange(pDHVInfo->fSeconds,0,60);//modf liu
		FALSEReturn(bCheck,CUNPACK_NULL);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2 + 1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pDHVInfo->fVel = atof(f_szTemp2 + 1);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2 + 1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pDHVInfo->velX = atof(f_szTemp2 + 1);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{;//modf liu
	BOOL bCheck = CheckDouble(f_szTemp2 + 1);
	FALSEReturn(bCheck,CUNPACK_NULL);
	pDHVInfo->velY = atof(f_szTemp2 + 1);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2 + 1);//modf liu
		FALSEReturn(bCheck,CUNPACK_NULL);

		pDHVInfo->velZ = atof(f_szTemp2 + 1);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{;//modf liu
	BOOL bCheck = CheckDouble(f_szTemp2 + 1);;//modf liu
	FALSEReturn(bCheck,CUNPACK_NULL);
	;//modf liu
	pDHVInfo->velGrnd = atof(f_szTemp2 + 1);;//modf liu
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2 + 1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		pDHVInfo->velMax = atof(f_szTemp2 + 1);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2 + 1);;//modf liu
		FALSEReturn(bCheck,CUNPACK_NULL);

		pDHVInfo->velAve = atof(f_szTemp2 + 1);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2 + 1);;//modf liu
		FALSEReturn(bCheck,CUNPACK_NULL);

		pDHVInfo->velCourseAve = atof(f_szTemp2 + 1);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2 + 1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		pDHVInfo->velValid = atof(f_szTemp2 + 1);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',' && f_szTemp2[1] != '*')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);//
		FALSEReturn(bCheck,CUNPACK_NULL);

		pDHVInfo->chUnit = f_szTemp2[1];
	}

	Len = sizeof(DHVInfo);

	return CUNPACK_DHV;
}

int CRdss2_1SearchExp::explain_xsd(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	BYTE * pBytes = (BYTE*)byFrame;
	unsigned int uTemp = 0;
	int it = 8;//2014-01-07 lxl cause of include '$'
	LPSubUserPos pUsrPos = (LPSubUserPos)DataBuff;
	memset(pUsrPos,0,sizeof(SubUserPos));

	pUsrPos->nUsrId |= byFrame[it++];
	pUsrPos->nUsrId <<= 8;
	pUsrPos->nUsrId |= byFrame[it++];
	pUsrPos->nUsrId <<= 8;
	pUsrPos->nUsrId |= byFrame[it++];

	uTemp = byFrame[it++];
	pUsrPos->byPrecisionInd = (uTemp & 0x80)>>7;
	pUsrPos->bUrgent        = (uTemp & 0x40)>>6;
	pUsrPos->bMutiVal       = (uTemp & 0x20)>>5;
	pUsrPos->bGaoKong       = (uTemp & 0x10)>>4;			

	uTemp = byFrame[it++];
	uTemp<<=8;
	uTemp |= byFrame[it++];
	uTemp<<=8;
	uTemp |= byFrame[it++];
	uTemp<<=8;
	uTemp |= byFrame[it++];
	//shijian
	pUsrPos->byPosHour = uTemp >> 24;
	pUsrPos->byPosMin  = (uTemp >> 16)&0xFF;
	pUsrPos->fPosSec   = (uTemp >> 8)&0xFF;
	pUsrPos->fPosSec  += ((uTemp)&0xFF) * 0.01;

	uTemp = byFrame[it++];
	uTemp<<=8;
	uTemp |= byFrame[it++];
	uTemp<<=8;
	uTemp |= byFrame[it++];
	uTemp<<=8;
	uTemp |= byFrame[it++];
	//jingdu
	pUsrPos->dfLong = uTemp >> 24;
	pUsrPos->dfLong += ((uTemp >> 16) & 0xFF)/60.0;
	pUsrPos->dfLong += ((uTemp >> 8) & 0xFF)/3600.0;
	pUsrPos->dfLong += ((uTemp) & 0xFF)/36000.0;

	uTemp = byFrame[it++];
	uTemp<<=8;
	uTemp |= byFrame[it++];
	uTemp<<=8;
	uTemp |= byFrame[it++];
	uTemp<<=8;
	uTemp |= byFrame[it++];
	//weidu
	pUsrPos->dfLat = uTemp >> 24;
	pUsrPos->dfLat += ((uTemp >> 16) & 0xFF)/60.0;
	pUsrPos->dfLat += ((uTemp >> 8) & 0xFF)/3600.0;
	pUsrPos->dfLat += ((uTemp) & 0xFF)/36000.0;

	uTemp = byFrame[it++];
	uTemp<<=8;
	uTemp |= byFrame[it++];
	pUsrPos->fHgt = (uTemp & 0x3FFF);
	if(uTemp & 0x4000)
	{
		pUsrPos->fHgt *= -1;
	}

	uTemp = byFrame[it++];
	uTemp<<=8;
	uTemp |= byFrame[it++];
	pUsrPos->fHgtXcept = (uTemp & 0xFF);

	if(uTemp & 0x0100)
	{
		pUsrPos->fHgtXcept *= -1;
	}

	Len = sizeof(SubUserPos);

	return CUNPACK_XSD;
}

int CRdss2_1SearchExp::explain_xst(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;
	
//	unsigned int uData = 0;
LPSubUserMsg pSubUserMsg = (LPSubUserMsg)DataBuff;
memset(pSubUserMsg,0,sizeof(SubUserMsg));
//	BYTE * pBytes = (BYTE*)byFrame;
//	int it = 8;//2014-01-07 lxl cause of  '$' included
//
//	pSubUserMsg->nRcvrId = byFrame[it++];
//	pSubUserMsg->nRcvrId <<= 8;
//
//	pSubUserMsg->nRcvrId |= byFrame[it++];
//	pSubUserMsg->nRcvrId <<= 8;
//
//	pSubUserMsg->nRcvrId |= byFrame[it++];
//
//	uData = byFrame[it++];
////	pTXRInfo->byInfoType = byTemp&8 ? 4 : 1;
//
//	//pTXRInfo->byTeleFormat = byTemp&0x20 ? 1 : 0;
//	pSubUserMsg->byCodeType = (uData >> 7);//
//	//
//	pSubUserMsg->nSendId = byFrame[it++];//
//	pSubUserMsg->nSendId <<= 8;////
//
//	pSubUserMsg->nSendId |= byFrame[it++];
//	pSubUserMsg->nSendId <<= 8;
//
//	pSubUserMsg->nSendId |= byFrame[it++];
//
//	pSubUserMsg->nSendHour= byFrame[it++];
//	pSubUserMsg->nSendMin = byFrame[it++];
//
//	uData = byFrame[it++];
//	uData<<=8;
//	uData |= byFrame[it++];
//	pSubUserMsg->nCommBitsLen = uData;
//
//	size_t i = 0;			
//
//	for(i = 0 ; i < (uData/8) && i < sizeof(pSubUserMsg->chContent);i++)
//	{
//		pSubUserMsg->chContent[i] = byFrame[it++];
//	}
//
//	if(pSubUserMsg->byCodeType == 1 && (pSubUserMsg->nCommBitsLen%8) != 0)
//	{
//		pSubUserMsg->chContent[i] = byFrame[it++];
//		pSubUserMsg->chContent[i] &= 0xF0;
//	}
//
//	pSubUserMsg->byCrcFlag = byFrame[it++];
//
//	Len = sizeof(SubUserMsg);
	int  iter = 8;
	int  uTemp = 0 ;
	BYTE byTemp;

	//发信方地址
	//__readint_3bytes(byFrame,iter,uTemp);
	//pSubUserMsg->nRcvrId = uTemp;

	pSubUserMsg->nRcvrId = (unsigned char)byFrame[iter++];//
	pSubUserMsg->nRcvrId <<= 8;////

	pSubUserMsg->nRcvrId |= (unsigned char)byFrame[iter++];
	pSubUserMsg->nRcvrId <<= 8;

	pSubUserMsg->nRcvrId |= (unsigned char) byFrame[iter++];


	__read_byte(byFrame,iter,byTemp);
	pSubUserMsg->byTeleFormat = byTemp&8 ? 4 : 1;

	pSubUserMsg->byCodeType = byTemp&0x20 ? 1 : 0;

	pSubUserMsg->nSendId = (unsigned char)byFrame[iter++];//
	pSubUserMsg->nSendId <<= 8;////
	
	pSubUserMsg->nSendId |= (unsigned char)byFrame[iter++];
	pSubUserMsg->nSendId <<= 8;

	pSubUserMsg->nSendId |= (unsigned char) byFrame[iter++];

	__read_byte(byFrame,iter,byTemp);
	pSubUserMsg->nSendHour = byTemp;

	__read_byte(byFrame,iter,byTemp);
	pSubUserMsg->nSendMin = byTemp;

	__readint_2bytes(byFrame,iter,uTemp);
	
	BYTE flag;
	__read_byte(byFrame,iter,flag);
	iter--;
	if(pSubUserMsg->byTeleFormat == 1){//bcd
		if(flag != 0xA0 && flag!= 0xA4)
		{//bcd
			int chars = bytes_hexchars(pSubUserMsg->chContent,sizeof(pSubUserMsg->chContent),(unsigned char*)&byFrame[iter],uTemp);
			pSubUserMsg->nCommLen = chars;
			pSubUserMsg->chContent[chars] = '\0';
		}else if(flag == 0xA0){
			return FALSE;
		}else if(flag == 0xA4){
			//混发通信
			while(uTemp%8 != 0) uTemp--;
			pSubUserMsg->byTeleFormat = 2;
			memcpy_s(pSubUserMsg->chContent,sizeof(pSubUserMsg->chContent),&byFrame[iter+1],uTemp/8 -1);
			pSubUserMsg->nCommLen = uTemp/8 -1;
			pSubUserMsg->chContent[pSubUserMsg->nCommLen] = '\0';
		}

	}else{
		//汉字通信
		while(uTemp%8 != 0) uTemp--;		
		memcpy_s(pSubUserMsg->chContent,sizeof(pSubUserMsg->chContent),&byFrame[iter],uTemp/8);
		pSubUserMsg->nCommLen = uTemp/8;
		pSubUserMsg->chContent[pSubUserMsg->nCommLen] = '\0';
	}

	iter+=uTemp/8 + (uTemp%8?1:0);

	__read_byte(byFrame,iter,byTemp);
	pSubUserMsg->byCrcFlag = byTemp;

	return CUNPACK_XST;
}

int CRdss2_1SearchExp::explain_rns(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	LPRNSAck  pRNSAck = (LPRNSAck)DataBuff;
	memset(pRNSAck,0,sizeof(RNSAck));

	f_szTemp2 = strstr((char*)byFrame, ",");

	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',' && f_szTemp2[1] != '*')
	{
		BOOL bCheck = CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		pRNSAck->nPackNext = atoi(f_szTemp2+1);
	}

	Len = sizeof(RNSAck);

	return CUNPACK_RNS;
}

int CRdss2_1SearchExp::explain_apl(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	string strInfo;

	APLInfo *pAPLInfo = (APLInfo*)DataBuff;
	memset(pAPLInfo,0,sizeof(APLInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);			

	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		int  nFieldLen = FieldLength(f_szTemp2+1);//
		bCheck = (nFieldLen > 5);
		FALSEReturn(bCheck,CUNPACK_NULL);

		bCheck = CheckDouble(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		if(nFieldLen == 6)
			FALSEReturn(CheckInt(f_szTemp2+1),CUNPACK_NULL);

		strInfo = f_szTemp2 +1;
		FALSEReturn(strInfo.size()>3,CUNPACK_NULL);

		pAPLInfo->nHour   = atoi(strInfo.substr(0,2).c_str());
		pAPLInfo->nMinute = atoi(strInfo.substr(2,2).c_str());
		const char * pstr = strInfo.c_str();
		pAPLInfo->fSecond = atof(pstr + 4);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pAPLInfo->LevelH = atof(f_szTemp2+1);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckDouble(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pAPLInfo->LevelV = atof(f_szTemp2+1);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);


	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ','&&f_szTemp2[1] != '*')
	{
		BOOL bCheck = CheckDouble(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pAPLInfo->Level3D = atof(f_szTemp2+1);
	}

	Len = sizeof(APLInfo);

	return CUNPACK_APL;
}

int CRdss2_1SearchExp::explain_bid(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	BIDInfo* pBidInfo = (BIDInfo*)DataBuff;
	memset(pBidInfo,0,sizeof(BIDInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = CheckInt(f_szTemp2 +1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pBidInfo->nFrec = atoi(f_szTemp2 +1);
		bCheck = CheckCloseRange(pBidInfo->nFrec,1,7);
		FALSEReturn(bCheck,CUNPACK_NULL);
	}
	else
		return CUNPACK_NULL;

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',' && f_szTemp2[1] != '*')
	{
		BOOL bCheck = FALSE;
		bCheck = (FieldLength(f_szTemp2+1) == 1);
		FALSEReturn(bCheck,CUNPACK_NULL);

		pBidInfo->cCP = f_szTemp2[1] ;
		char chSet[3] = {'C','P','A'};
		bCheck = CheckInSet(pBidInfo->cCP,chSet,3);
		FALSEReturn(bCheck,CUNPACK_NULL);
	}
	else
		pBidInfo->cCP = '\0';

	Len = sizeof(BIDInfo);

	return CUNPACK_BID;
}

int CRdss2_1SearchExp::explain_zhr(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	int  f_iInfoID = 0;
	int  f_iAppendIndex = 0;
	char *f_szTemp2 = NULL;
	int  iTitleIter = 0;

	ZHRInfo * pInfo = (ZHRInfo*)DataBuff;
	memset(pInfo,0,sizeof(ZHRInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		for(int i = 0 ; i < sizeof(pInfo->ZHInfo) && f_szTemp2[1+i] != '*';i++)
		{
			pInfo->ZHInfo[i] = f_szTemp2[1+i];
		}
	}
	Len = sizeof(ZHRInfo);

	return CUNPACK_ZHR;
}

int CRdss2_1SearchExp::GetTitleID(const char byFrame[])
{
	int  iTitleIter = 0;

	for(iTitleIter=0; iTitleIter<RDSSCOUNT; iTitleIter++)
	{
		if(memcmp(byFrame+2+1, m_titile[iTitleIter].cTitle, 3)==0)
			break;
	}

	TRUEReturn(iTitleIter==0 || iTitleIter >=RDSSCOUNT,CUNPACK_NULL);

	return m_titile[iTitleIter].iInfoID;
}

int CRdss2_1SearchExp::encode_rmo(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen)
{
	DWORD f_EncodeDataLen=0;
	lpRMOInfo pRMOInfo = (lpRMOInfo)pMsg;
	f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCRMO,%s,%d,%.1f",
		pRMOInfo->chName,pRMOInfo->byMode,(float)(pRMOInfo->nOutFreq));
	return f_EncodeDataLen;
}
