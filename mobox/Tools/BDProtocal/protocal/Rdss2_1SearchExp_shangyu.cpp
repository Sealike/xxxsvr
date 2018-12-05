// Rdss2_1SearchExp.cpp: implementation of the CRdss2_1SearchExp_shangyu class.
//
//////////////////////////////////////////////////////////////////////

#include "Rdss2_1SearchExp_shangyu.h"
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
//��Ϣ�������弰��ͷӳ�䶨��
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
	{CUNPACK_BSI,"BSI"},//������Ϣ
	{CUNPACK_DWR,"DWR"},//��λ��Ϣ
	{CUNPACK_FKI,"FKI"},//������Ϣ
	{CUNPACK_ICI,"ICI"},//�ӽ���ģ����Ϣ
	{CUNPACK_ICZ,"ICZ"},//ָ�ӻ������û���Ϣ
	{CUNPACK_KLS,"KLS"},//����ʶ��Ӧ��
	{CUNPACK_HZR,"HZR"},//��ִ��Ϣ
	{CUNPACK_TXR,"TXR"},//ͨ����Ϣ
	{CUNPACK_SBX,"SBX"},//�豸��ز���
	{CUNPACK_ZTI,"ZJX"},//�û��豸״̬��Ϣ
	{CUNPACK_ZDA,"ZDA"}, //UTCʱ�䣬���ڣ��ͱ���ʱ������Ϣ
	{CUNPACK_LZM,"LZM"},//�豸��ֵ��Ϣ��
	{CUNPACK_WAA,"WAA"},//λ�ñ���1
	{CUNPACK_WBA,"WBA"},//λ�ñ���2
	{CUNPACK_XSD,"XSD"},//������λ��Ϣ
	{CUNPACK_XST,"XST"},//����ͨ����Ϣ
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
//���캯��
////////////////////////////////////////////////////////////////
CRdss2_1SearchExp_shangyu::CRdss2_1SearchExp_shangyu()
{
	SetRdssTitles();
}

////////////////////////////////////////////////////////////////
//��������
////////////////////////////////////////////////////////////////
CRdss2_1SearchExp_shangyu::~CRdss2_1SearchExp_shangyu()
{

}

////////////////////////////////////////////////////////////////
//�ж��ǲ���Э��֡
////////////////////////////////////////////////////////////////
//BOOL CRdss2_1SearchExp_shangyu::IsValidTitles(char* strTitle)
//{
//	itStr itBgn = m_vRdssTitle.begin();
//	itStr itEnd = m_vRdssTitle.end();
//
//	itStr itFind = find(itBgn,itEnd,strTitle);
//	if(itFind != itEnd)
//		return TRUE;
//	else
//		return FALSE;
//}

////////////////////////////////////////////////////////////////
//��ʼ��Э������
////////////////////////////////////////////////////////////////
void CRdss2_1SearchExp_shangyu::SetRdssTitles()
{
	m_vRdssTitle.push_back("$BDGSV");//
	m_vRdssTitle.push_back("$BDRMC");//
	m_vRdssTitle.push_back("$BDGGA");//
	m_vRdssTitle.push_back("$BDDHV");//
	m_vRdssTitle.push_back("$BDGSA");//
	m_vRdssTitle.push_back("$BDZDA"); //UTCʱ�䣬���ڣ��ͱ���ʱ������Ϣ
	m_vRdssTitle.push_back("$BDBSI");//������Ϣ
	m_vRdssTitle.push_back("$BDVTG");//������Ϣ
	m_vRdssTitle.push_back("$BDDWR");//��λ��Ϣ
	m_vRdssTitle.push_back("$BDFKI");//������Ϣ
	m_vRdssTitle.push_back("$BDICI");//�ӽ���ģ����Ϣ
	m_vRdssTitle.push_back("$BDICZ");//ָ�ӻ������û���Ϣ
	m_vRdssTitle.push_back("$BDKLS");//����ʶ��Ӧ��
	m_vRdssTitle.push_back("$BDHZR");//��ִ��Ϣ
	m_vRdssTitle.push_back("$BDTXR");//ͨ����Ϣ
	m_vRdssTitle.push_back("$BDZJX");//�û��豸״̬��Ϣ
	m_vRdssTitle.push_back("$BDWAA");//λ�ñ���1
	m_vRdssTitle.push_back("$BDXSD");//������λ��Ϣ
	m_vRdssTitle.push_back("$BDXST");//����ͨ����Ϣ
	m_vRdssTitle.push_back("$BDSBX");//�豸��ز���
	m_vRdssTitle.push_back("$BDLZM");//�豸��ֵ��Ϣ��
	m_vRdssTitle.push_back("$BDGXM");//
	m_vRdssTitle.push_back("$BDPKY");//
	m_vRdssTitle.push_back("$BDICM");//
	m_vRdssTitle.push_back("$BDRNS");//
	m_vRdssTitle.push_back("$BDPMU");//
	m_vRdssTitle.push_back("$BDBID");//
	m_vRdssTitle.push_back("$BDZHR");//
}

////////////////////////////////////////////////////////////////
//����û������Ƿ��Ǻ����
////////////////////////////////////////////////////////////////
BOOL CRdss2_1SearchExp_shangyu::CheckUserData(const void * pMsg,const UINT uMsgId)
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
		{//������ѯ
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
		{//��Ĭ����
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
		//14 RDSS ����豸��
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
		{//������Ϣ
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
		//6.RDSS��λ��������ָ��
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
		//7.RDSS��ʱ��������ָ��
	case DEVICE_BDDSA:
		{//��ʱ��������ָ��
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
		//8.RDSSͨ�Ų�ѯָ��
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
///Process ���ݴ���
///////////////////////////////////////////////////////////////////////////////
int CRdss2_1SearchExp_shangyu::explain_zti(const char byFrame[] , int nFrameLen,const char *DataBuff, DWORD &Len)
{
	string strInfo;
	char *f_szTemp2 = NULL;
	//caution
	lpZTIInfo pZTIInfo = (lpZTIInfo)DataBuff;
	memset(pZTIInfo,0,sizeof(ZTIInfo));

	f_szTemp2 = strstr((char*)byFrame, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck=CheckInt(f_szTemp2+1);   //Modf GJ
		FALSEReturn(bCheck,CUNPACK_NULL);//caution
		pZTIInfo->data.selfcheck.ICCardState=atoi(&f_szTemp2[1]);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck=CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pZTIInfo->data.selfcheck.HardState=atoi(&f_szTemp2[1]);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		//bCheck = (FieldLength(f_szTemp2+1) == 1);//modf liu
		bCheck=CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);//modf liu
		pZTIInfo->data.selfcheck.Batterystate=atoi(&f_szTemp2[1]);
	}

	f_szTemp2 = strstr(f_szTemp2+1, ",");
	NULLReturn(f_szTemp2,CUNPACK_NULL);
	if(f_szTemp2[1] != ',')
	{
		BOOL bCheck = FALSE;
		bCheck=CheckInt(f_szTemp2+1);
		FALSEReturn(bCheck,CUNPACK_NULL);
		pZTIInfo->data.selfcheck.InState=atoi(&f_szTemp2[1]);
	}
	BeamInfo beam;
	for(int i=0;i<10;i++)
	{
		f_szTemp2 = strstr(f_szTemp2+1, ",");
		NULLReturn(f_szTemp2,CUNPACK_NULL);
		if(f_szTemp2[1] != ',')
		{//modf liu
			BOOL bCheck = FALSE;//modf liu
			bCheck = (FieldLength(f_szTemp2+1) == 1);//
			FALSEReturn(bCheck,CUNPACK_NULL);//modf liu
			bCheck=CheckInt(f_szTemp2+1);
			FALSEReturn(bCheck,CUNPACK_NULL);
			pZTIInfo->data.selfcheck.PowerState[i]= atoi(&f_szTemp2[1]);
			beam.byPower[i]=atoi(&f_szTemp2[1]);
		}
	}
	Len = sizeof(ZTIInfo);

	return CUNPACK_ZTI;
}

int CRdss2_1SearchExp_shangyu::encode_rmo(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen)
{
	int f_EncodeDataLen=0;
	lpRMOInfo pRMOInfo = (lpRMOInfo)pMsg;
	if(strcmp(pRMOInfo->chName,"ZTI") == 0)
	{
		strcpy(pRMOInfo->chName,"ZJX");
	}
	else if(strcmp(pRMOInfo->chName,"ZDA") == 0)
	{
		strcpy(pRMOInfo->chName,"ZDA");
	}
	else if(strcmp(pRMOInfo->chName,"BSI") == 0)
	{
		strcpy(pRMOInfo->chName,"BSI");
	}
	f_EncodeDataLen = sprintf_s((char*)byFrameBuff,nFrameBuffLen,"$CCRMO,%s,%d,%.1f",
		pRMOInfo->chName,pRMOInfo->byMode,(float)(pRMOInfo->nOutFreq));
	return f_EncodeDataLen;
}

int CRdss2_1SearchExp_shangyu::GetTitleID(const char byFrame[])
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
