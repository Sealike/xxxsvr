#include "Rdss4_0SearchExp.h"
#include "GlobalFunction.h"
#include "../../BeiDouDesktop/Tool.h"

#include <algorithm>
#include <string.h>
#include <vector>
#include <assert.h>

using namespace std;

SInfoTitle RDSS4_0Title[] =
{
	{CUNPACK_HZR,"$TXHZ"},
	{CUNPACK_SBX,"$BBXX"},
	{CUNPACK_GXM,"$GLXX"},
	{CUNPACK_LZM,"$LZXX"},
	{CUNPACK_GGA,"$GNPX"},
	{CUNPACK_BSI,"$GLZK"},
	{CUNPACK_ZTI,"$ZJXX"},//波束信息
	{CUNPACK_DWR,"$DWXX"},//定位信息
	{CUNPACK_FKI,"$FKXX"},//反馈信息
	{CUNPACK_ICI,"$ICXX"},//加解密模块信息
	{CUNPACK_TXR,"$TXXX"},//通信信息
	{CUNPACK_ZTI,"$ZJXX"},//用户设备状态信息
	{CUNPACK_ZDA,"$GNTX"}, //UTC时间，日期，和本地时区等信息	GJ 2016.3.29
};

GGAInfo  g_ggaPos;

BOOL CRdss4_0SearchExp::AddExtenedPlugin(int msgid,char* title,pCBExt_explain exp_cb)
{
	AUTOLOCK(g_lock_plugin);

	BOOL rtn = FALSE;
	
	if(msgid <=0 || title == NULL || exp_cb == NULL) return FALSE;

	size_t i = 0;
	for(; i < m_plugins.size(); i++)
	{
		if(m_plugins[i].id == msgid) break;
		if(strcmp(title,m_plugins[i].title) == 0) break;		
	}

	if(i < m_plugins.size())
	{
		extendedplugin_st plugin;

		plugin.id = msgid;
		plugin.title = _strdup(title);
		plugin.pExpalin = exp_cb;
		plugin.param = this;
		m_plugins.push_back(plugin);

		rtn = TRUE;
	}

	return rtn;
}

int CRdss4_0SearchExp::CheckTitleType(char* title)
{
	int i = 0 ;

	for(; i < sizeof(RDSS4_0Title)/sizeof(SInfoTitle);i++){
		if(strcmp(title,RDSS4_0Title[i].cTitle) == 0){
			break;
		}
	}

	if(i < sizeof(RDSS4_0Title)/sizeof(SInfoTitle))
		return RDSS4_0Title[i].iInfoID;
	else{		
		return CUNPACK_NULL;
	}
}

CRdss4_0SearchExp::CRdss4_0SearchExp()
{
	m_dwComBufLen  = 0;
	m_dwCurrPt     = 0;
	m_bSearchStatus= SEARCHMACHINE_NULL;
	m_bCheckSum    = 0;
	m_wFrameIndLen = 0;
	m_uBinaryLen   = 0;
	m_uLocalID = 0;
	m_ici = new tagICIInfo;
	memset(m_ici,0,sizeof(tagICIInfo));	
	SetRdssTitles();
}

CRdss4_0SearchExp::~CRdss4_0SearchExp()
{

}

int CRdss4_0SearchExp::ProcessData(const BYTE byBuffData[], const int nDataLen)
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

				BYTE byType = DataExplain(m_FrameBuff, m_dwFrameLen,m_msgCache,msglen);
				if(byType != CUNPACK_NULL){
					NotifyMsg(byType,(char*)m_msgCache,msglen);

					if(byType == CUNPACK_ZTI){
						NotifyMsg(CUNPACK_BSI,(char*)m_msgCache+sizeof(ZTIInfo),sizeof(BeamInfo));		
					}
				}else{
					BYTE byType = excuteplugin(m_FrameBuff, m_dwFrameLen,m_msgCache,msglen);
					if(byType != CUNPACK_NULL)
					{
						NotifyMsg(byType,(char*)m_msgCache,msglen);
					}
				}
				
			}while(f_bSearchRes==PACK_STILLDATA);

			nProcessLen += nThisLen;
		}

	}catch(...)
	{
		//AfxMessageBox(_T("FireException!"));
		//TRACE1("%s","FireException");
	}

	return 1;
}

BYTE CRdss4_0SearchExp::SearchData()
{
#define __HEADLEN  5
	for(BYTE ch; m_dwCurrPt<m_dwComBufLen; m_dwCurrPt++)
	{
		ch = m_ComBuff[m_dwCurrPt];

		switch(m_bSearchStatus)
		{
		case SEARCHMACHINE_NULL:	//  0: $
			{
				static char Head[__HEADLEN+1];
				static int  HeadPtr = 0;
				Head[HeadPtr++]     = ch;
				if(HeadPtr == __HEADLEN)
				{//搜索头部
					Head[__HEADLEN] = '\0';
					if(IsRdssTitles(Head))
					{
						m_bSearchStatus = SEARCHMACHINE_LENGTH;
						m_bCheckSum  = 0;//不包括$.
						memcpy(m_FrameBuff,Head,__HEADLEN);

						for(int check =0;check < __HEADLEN;check++)
							m_bCheckSum ^= m_FrameBuff[check];																				

						m_dwFrameLen = __HEADLEN;
						HeadPtr = 0;
						break;
					}else
					{
						int i = 1;
						for(; i<__HEADLEN;i++)						
							if(Head[i] == '$')					
								break;													

						if(i<__HEADLEN)
						{
							for(int j = 0 ; j < __HEADLEN-i ; j++)
								Head[j] = Head[j+i];							

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

			if(m_dwFrameLen == 7)//获取二进制的协议长度。
			{
				m_uBinaryLen = m_FrameBuff[5]*256 + m_FrameBuff[6];
				if(m_uBinaryLen >= UNPACKBUFFSIZE-1)
				{
					m_bSearchStatus = SEARCHMACHINE_NULL;
					continue;
				}else{
					m_bSearchStatus = SEARCHMACHINE_BINARYCHECK;
					continue;
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

				if(m_dwFrameLen  == m_uBinaryLen)//修改于2014-01-06 ,lxl
				{
					if(0 == m_bCheckSum)
					{
						if(m_dwCurrPt==(m_dwComBufLen-1))
						{//找到完整的一帧数据,且缓冲区中只有一帧数据
							m_dwCurrPt = 0;
							m_dwComBufLen = 0;
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
	}else{
		return PACK_UNFINISHED;
	}
}

BOOL CRdss4_0SearchExp::IsRdssTitles(char* strTitle)
{
	itStr itBgn = m_vRdssTitle.begin();
	itStr itEnd = m_vRdssTitle.end();

	itStr itFind = find(itBgn,itEnd,strTitle);
	if(itFind != itEnd)
		return TRUE;
	else{
		size_t j = 0;
		for(;j < m_plugins.size();j++){
			if(strcmp(m_plugins[j].title,strTitle) == 0)
				break;
		}

		if(j == m_plugins.size())
			return FALSE;
		else
			return TRUE;
	}
}

////////////////////////////////////////////////////////////////
//初始化协议数组
////////////////////////////////////////////////////////////////
void CRdss4_0SearchExp::SetRdssTitles()
{
	m_vRdssTitle.push_back("$LZXX");
	m_vRdssTitle.push_back("$GNPX");//位置信息
	m_vRdssTitle.push_back("$DWXX");//定位信息
	m_vRdssTitle.push_back("$TXXX");//通信信息
	m_vRdssTitle.push_back("$GLZK");//功率状况
	m_vRdssTitle.push_back("$ZJXX");//自检信息
	m_vRdssTitle.push_back("$GLXX");//管理信息
	m_vRdssTitle.push_back("$ILXX");//I路信息
	m_vRdssTitle.push_back("$QLXX");//Q路信息
	m_vRdssTitle.push_back("$ICXX");//IC信息
	m_vRdssTitle.push_back("$ZHQR");//自毁确认
	m_vRdssTitle.push_back("$GNTX");//UTC时间，日期，和本地时区等信息  GJ 2016.3.39
	m_vRdssTitle.push_back("$FKXX");//反馈信息
	m_vRdssTitle.push_back("$BBXX");//版本信息
	m_vRdssTitle.push_back("$TXHZ");//回执信息 GJ 2016.3.39
	
}

BYTE CRdss4_0SearchExp::Checksum(BYTE byBuffer[],int len)
{
	BYTE byRtn = 0;

	assert(byBuffer && len > 0);
	if(byBuffer==NULL || len <= 0) return 0;

	for(int i = 0 ; i < len ; i++)
		byRtn ^= byBuffer[i];
	
	return byRtn;
}
static int BCDCompress(BYTE DestBuffer[],int nDestBufferlen,char chSrcBuff[],int nSrclen)
{
	int bitcnt=0;
	BYTE data;

	int predicate = nSrclen/2 + nSrclen%2;
	if(predicate > nDestBufferlen)
		return 0;

	for(int i = 0;i<nSrclen;i++)
	{
		__char_hex(chSrcBuff[i],data);
		if(i%2==0)
			DestBuffer[bitcnt/8] = data <<4;
		else if(i%2==1)
		{
			DestBuffer[bitcnt/8] |= data&0x0F;			
		}

		bitcnt+=4;
	}

	return bitcnt;
}

//地址转换
static int addressConvert(char* destBuff,char* SrcBuff)
{
	assert(SrcBuff);
	size_t nLen=0;
	for(size_t i=0;i<strlen(SrcBuff);i++)
	{
		if(SrcBuff[nLen]!='.'||SrcBuff[i]!=':')
		{
			destBuff[nLen]=SrcBuff[i];
			nLen++;
		}
	}
	return nLen;
}

int CRdss4_0SearchExp::adjustcontent(char content[],int contentlen/*bytes*/,int codetype,BOOL urgent,outbuffer_t data)
{
	int combits = 0;
	int capbits ;
	CToolClass<lpICIInfo> tool(m_ici);
	capbits = tool.GetCommBitsLen(urgent);
	if(capbits == 0)
		return 0;

	if(codetype==2){//混发通信	
		combits = (contentlen+1) * 8;//a4
		if(capbits < combits)
		{
			combits = capbits;
			combits = combits/8*8;
		}
		data[0] = (unsigned char)0xA4;
		memcpy(&data[1],content,combits/8-1);
	}else if(codetype==1)
	{//bcd
		BYTE buffer[1024];
		memset(buffer,0,sizeof(buffer));
		combits=BCDCompress(buffer,sizeof(buffer),content,contentlen);
		if(capbits < combits)
		{
			combits = capbits;			
		}
		combits = combits/4*4;
		memcpy(data,buffer,combits/8+(combits%8?1:0));
	}else{//汉字通信
		combits=contentlen*7;
		if(capbits < combits)
		{
			combits = capbits;	
			combits = combits/14*14;
		}
		combits = combits/7*8;
		memcpy(data,content,combits/8);
	}

	return combits;
}

int CRdss4_0SearchExp::EncodeStopInstruct(char* title,int type,unsigned char frame[],int &len)
{
	int rtn = 0;

	if(title == NULL||frame == NULL) return 0;

	len = 0;
	if(type == 1){
		memcpy(frame,"$JSZL",5);
		len += 5;

		len += 2;
		__writeint_3bytes(frame,len,m_uLocalID);
		for(int i = 0 ; i < 4 && title[i] != '\0'; i++){
			frame[len++] = title[i];
		}

		int pos = 5;
		__writeint_2bytes(frame,pos,len+1);

		frame[len]=Checksum((unsigned char*)frame,len);
		len++;
		

	}else if(type == 3){
		memcpy(frame,"$JSZL",5);
		len += 5;

		len += 2;
		__writeint_3bytes(frame,len,m_uLocalID);
		for(int i = 0 ; i < 3 ; i++){
			frame[len++] = 0x55;
		}

		int pos = 5;
		__writeint_2bytes(frame,pos,len+1);

		frame[len]=Checksum((unsigned char*)frame,len);
		len++;
		
	}

	return len;
}

int  CRdss4_0SearchExp::EncodeFrame_rmo(lpRMOInfo pRmoInfo ,unsigned char frame[],int& len)
{
	int nBufferLen = 0;

	len = 0;
	if(pRmoInfo->byMode == 3){//结束全部
		nBufferLen = EncodeStopInstruct(NULL,3,frame,len);
		return nBufferLen;
	}

	if(memcmp(pRmoInfo->chName,"ZDA",3) == 0){//时间输出
		if(pRmoInfo->byMode == 2){
			memcpy(frame+len,"$GNPS",sizeof("$GNPS")); //GJ 2016.3.29
			len+=5;

			len+=2;
			__writeint_3bytes(frame,len,m_uLocalID);

			frame[len++]=0x08;        //GJ 2016.3.29

			__writeint_2bytes(frame,len,pRmoInfo->nOutFreq);
			
			int pos = 5;
			__writeint_2bytes(frame,pos,len+1);

			frame[len]=Checksum(frame,len);

			len++;			
			nBufferLen = len;
			
		}else if(pRmoInfo->byMode == 1){
			nBufferLen = EncodeStopInstruct("GNPS",1,frame,len); //GJ 2016.3.29
		}

	}else if(memcmp(pRmoInfo->chName,"BSI",3) == 0)//功率检测
	{
		if(pRmoInfo->byMode == 2){			
			memcpy(frame+len,"$GLJC",sizeof("$GLJC"));

			len+=5;
			len+=2;

			__writeint_3bytes(frame,len,m_uLocalID);
			__write_byte(frame,len,pRmoInfo->nOutFreq);	

			int pos = 5;
			__writeint_2bytes(frame,pos,len+1);

			frame[len++]=Checksum(frame,len);
			nBufferLen = len;			

		}else if(pRmoInfo->byMode == 1){
			nBufferLen = EncodeStopInstruct("GLJC",1,frame,len);
		}
	}else if(memcmp(pRmoInfo->chName,"ZTI",3) == 0){//系统自检
		if(pRmoInfo->byMode == 2)
		{
			memcpy(frame+len,"$XTZJ",sizeof("$XTZJ"));
			len+=5;
			len+=2;
			__writeint_3bytes(frame,len,m_uLocalID);
			__writeint_2bytes(frame,len,pRmoInfo->nOutFreq);

			int pos = 5;
			__writeint_2bytes(frame,pos,len+1);

			frame[len++]=Checksum(frame,len);

			nBufferLen = len;
			
		}else if(pRmoInfo->byMode == 1){
			nBufferLen = EncodeStopInstruct("XTZJ",1,frame,len);
		}

	}else if(memcmp(pRmoInfo->chName,"GGA",3) == 0){
		if(pRmoInfo->byMode == 2 || pRmoInfo->byMode == 1)
		{
			memcpy(frame+len,"$GNPS",sizeof("$GNPS"));
			len+=5;
			len+=2;
			__writeint_3bytes(frame,len,m_uLocalID);
			if(pRmoInfo->byMode == 2){
				__writeint_2bytes(frame,len,pRmoInfo->nOutFreq);
			}else if(pRmoInfo->byMode == 1){
				__writeint_2bytes(frame,len,0);
			}

			int pos = 5;
			__writeint_2bytes(frame,pos,len+1);

			frame[len++]=Checksum(frame,len);

			nBufferLen = len;			

		}
	}else if(memcmp(pRmoInfo->chName,"DWA",3) == 0)//定位申请
	{
		if(pRmoInfo->byMode == 1){
			nBufferLen = EncodeStopInstruct("DWSQ",1,frame,len);
		}
	}else if(memcmp(pRmoInfo->chName,"SBX",3) == 0)
	{
		const int nFramelen = 13;

		memcpy(frame+len,"$BBDQ",sizeof("$BBDQ"));
		len+=5;		
		len+=2;
		__writeint_3bytes(frame,len,m_uLocalID);

		int pos = 5;
		__writeint_2bytes(frame,pos,len+1);

		frame[len++]=Checksum(frame,len);

		nBufferLen = len;		
	}

	return nBufferLen;
}

int  CRdss4_0SearchExp::EncodeFrame(const void * pMsg,const UINT uMsgId,const int nMsgBlockLen, BYTE byFrameBuff[],const int nFrameBuffLen)
{
	assert(pMsg&&uMsgId>0);
	assert(byFrameBuff&&nFrameBuffLen>0);

	if(pMsg == NULL|| uMsgId<1 || byFrameBuff == NULL||nFrameBuffLen<1) return 0;
	int nBufferLen;
	switch(uMsgId)
	{
	case DEVICE_BDCXA:
		{
			lpBDCXA_STRUCT pCXAInfo = (lpBDCXA_STRUCT)pMsg;
			int iter = 0;
			unsigned char uchar;
			memcpy(byFrameBuff+iter,"$TXSQ",sizeof("$TXSQ"));
			iter+=5;

			iter+=2;
			__writeint_3bytes(byFrameBuff,iter,m_uLocalID);
			
			if(pCXAInfo->byQueryCat == 0){//定位查询
				uchar = 0x60;

				if(pCXAInfo->byQueryType == 1)//一次
					uchar |= 0x00;
				else if(pCXAInfo->byQueryType == 2)//二次
					uchar |= 0x04;
				else if(pCXAInfo->byQueryType == 3)//三次
					uchar |= 0x08;
				
				__write_byte(byFrameBuff,iter,uchar);
				__writeint_3bytes(byFrameBuff,iter,pCXAInfo->nUserId);
			}else if(pCXAInfo->byQueryCat == 1){//通信查询
				uchar = 0x70;
				//1：最新存入电文查询，用户地址为空；2：按发信地址查询，用户地址为发信方地址；3：回执查询，用户地址为收信方地址
				if(pCXAInfo->byQueryType == 1)
					uchar |= 0x00;
				else if(pCXAInfo->byQueryType == 2)
					uchar |= 0x04;
				else if(pCXAInfo->byQueryType == 3)
					uchar |= 0x08;

				__write_byte(byFrameBuff,iter,uchar);

				if(pCXAInfo->byQueryType == 1){
					__writeint_3bytes(byFrameBuff,iter,0);
				}else if(pCXAInfo->byQueryType == 2){
					__writeint_3bytes(byFrameBuff,iter,pCXAInfo->nUserId);
				}else if(pCXAInfo->byQueryType == 3)
					__writeint_3bytes(byFrameBuff,iter,pCXAInfo->nUserId);				
			}else
				return 0;
			
			int pos = 5;
			__writeint_2bytes(byFrameBuff,pos,iter+1);

			byFrameBuff[iter++] = Checksum(byFrameBuff,iter);
			nBufferLen = iter;			

		}
		break;
	case DEVICE_BDZHS:
		{
			LPZHSStruct pZHSInfo = (LPZHSStruct)pMsg;
			if(pZHSInfo->dwCmd == 0xAA5555AA){
				int iter = 0;
				memcpy(byFrameBuff+iter,"$ZHSQ",sizeof("$ZHSQ"));
				iter+=5;

				iter+=2;
				__writeint_2bytes(byFrameBuff,iter,m_uLocalID);
				__writeint_4bytes(byFrameBuff,iter,0x55AAAA55);

				int pos = 5;
				__writeint_2bytes(byFrameBuff,pos,iter+1);

				byFrameBuff[iter++] = Checksum(byFrameBuff,iter);
				nBufferLen = iter;				
				
			}
		}
		break;
	case DEVICE_BDJMS:
		{
			pBDJMS_STRUCT pJMSInfo = (pBDJMS_STRUCT)pMsg;
			int iter = 0;
			memcpy(byFrameBuff+iter,"$JMSZ",sizeof("$JMSZ"));
			iter += 5;

			iter += 2;

			__writeint_3bytes(byFrameBuff,iter,m_uLocalID);
			if(pJMSInfo->bInhabit){
				__write_byte(byFrameBuff,iter,0);
			}else{
				__write_byte(byFrameBuff,iter,1);
			}

			int pos = 5;
			__writeint_2bytes(byFrameBuff,pos,iter+1);

			byFrameBuff[iter] = Checksum(byFrameBuff,iter);
			iter++;
			nBufferLen = iter;			

		}
		break;
	case DEVICE_BDGXM:
		{
			int iter = 0;
			lpGXMInfo pGXMInfo = (lpGXMInfo)pMsg;
			if(pGXMInfo->nType == 1){
				memcpy(byFrameBuff+iter,"$GXZR",sizeof("$GXZR"));
				iter += 5;

				iter += 2;
				__writeint_3bytes(byFrameBuff,iter,m_uLocalID);
				__writeint_4bytes(byFrameBuff,iter,pGXMInfo->nSerialNo);
				int bits = AscToHex(byFrameBuff+iter,nFrameBuffLen-iter,pGXMInfo->chMagInfo,64);
				int bytes = bits%8 ? bits/8 : bits/8+1;
				iter += bytes;

				int pos = 5;
				__writeint_2bytes(byFrameBuff,pos,iter+1);

				byFrameBuff[iter] = Checksum(byFrameBuff,iter);
				iter++;
				nBufferLen = iter;
				
			}else if(pGXMInfo->nType == 2){
				memcpy(byFrameBuff+iter,"$GXDQ",sizeof("$GXDQ"));
				iter += 5;

				iter += 2;
				__writeint_3bytes(byFrameBuff,iter,m_uLocalID);

				int pos = 5;
				__writeint_2bytes(byFrameBuff,pos,iter+1);

				byFrameBuff[iter] = Checksum(byFrameBuff,iter);
				iter++;
				nBufferLen = iter;

				
			}

		}
		break;
	case DEVICE_BDRMO:
		{			
			int iter = 0;

			lpRMOInfo pRmoInfo = (lpRMOInfo)pMsg;
			nBufferLen = EncodeFrame_rmo(pRmoInfo,byFrameBuff,iter);
		}
		break;
	case DEVICE_BDICA:
		{
			int iter = 0;
			const int nFramelen = 12;
			pBDICA_STRUCT pBDICAinfo=(pBDICA_STRUCT)pMsg;

			memcpy(byFrameBuff+iter,"$ICJC",sizeof("$ICJC"));
			iter+=5;
			
			byFrameBuff[iter++] = nFramelen>>8;//frame len.
			byFrameBuff[iter++] = nFramelen&0xFF;

			if(pBDICAinfo->byType==0)
			{	
				pBDICAinfo->nSubUsrFrameNo=0;
				pBDICAinfo->nLocalID=0;				
			}
			else
			{
				pBDICAinfo->nLocalID = m_uLocalID;				
			}

			//local id.
			__writeint_3bytes(byFrameBuff,iter,pBDICAinfo->nLocalID);			
			//frame no.
			__write_byte(byFrameBuff,iter,pBDICAinfo->nSubUsrFrameNo);
			
			byFrameBuff[iter] = Checksum(byFrameBuff,iter);
			iter++;
			
			assert(iter == nFramelen);
			nBufferLen=nFramelen;
		}
		break;
	case DEVICE_BDTXA:
		{
			int iter = 0;
			int nFrameLen=18;
			lpTXAInfo pTXAInfo = (lpTXAInfo)pMsg;
			memcpy(byFrameBuff+iter,"$TXSQ",sizeof("$TXSQ"));
			iter+=7;

			//将用户机ID存入帧缓冲
			pTXAInfo->nLocalId = m_uLocalID;
			__writeint_3bytes(byFrameBuff,iter,pTXAInfo->nLocalId);
					
			//将通讯信息类别加入帧缓冲
			byFrameBuff[iter]=0x40;
			if(pTXAInfo->byCommCat == 1)
				byFrameBuff[iter]|=0x04;

			if(pTXAInfo->byCodeType != 0)
				byFrameBuff[iter]|= 0x02;

			if(pTXAInfo->ifcmd)
				byFrameBuff[iter]|=0x01;
			else
			{
				byFrameBuff[iter]|=0x04;
				pTXAInfo->ifAnswer=0;
				
			}
			iter++;

			//将用户地址存入帧缓冲
			__writeint_3bytes(byFrameBuff,iter,pTXAInfo->nUserId);

			iter+=2;
			
			//将是否应答存入帧缓冲
			__write_byte(byFrameBuff,iter,pTXAInfo->ifAnswer?1:0);

			//将电文内容存入帧缓冲中
			BYTE byBuffer[512];
			memset(byBuffer,0,sizeof(byBuffer));
			
			int combits = adjustcontent(pTXAInfo->chContent,pTXAInfo->nContentLen,pTXAInfo->byCodeType,pTXAInfo->byCommCat==0,(outbuffer_t)byBuffer);
			
			if(combits == 0) return 0;
			pTXAInfo->ifcmd = 0;//force..
			/*if(pTXAInfo->ifcmd==1)
			{
			if(pTXAInfo->byCodeType == 1 && pTXAInfo->chContent[0]!=0xA0&&pTXAInfo->chContent[0]!=0xA4)
			{
			if(comlen>83)
			comlen = 83/4*4;
			}
			else
			{
			if(comlen>83)
			{
			comlen = 83/8*8;
			}
			}
			}*/

			int byteLen=combits/8 + (combits%8?1:0);
			memcpy(byFrameBuff+iter,byBuffer,byteLen);
			iter+=byteLen;
			
			int totalByteLen=byteLen+18;

			int iterpos=5;
			//将字节总长度存入帧缓冲
			__writeint_2bytes(byFrameBuff,iterpos,totalByteLen);
			//将电文长度存入帧缓冲
			iterpos=14;
			__writeint_2bytes(byFrameBuff,iterpos,combits);
			byFrameBuff[iter++]=Checksum(byFrameBuff,iter);
			
			nBufferLen=iter;
		}
		break;
	case DEVICE_BDWAA:
		{			
			int iter = 0;
			lpWAAInfo pWAAInfo = (lpWAAInfo)pMsg;
			int nFrameLen=18;
			memcpy(byFrameBuff+iter,"$TXSQ",sizeof("$TXSQ"));
			iter+=7;

			//将用户机ID存入帧缓冲			
			__writeint_3bytes(byFrameBuff,iter,m_uLocalID);
			//将通讯信息类别加入帧缓冲
			byFrameBuff[iter]=0x40;	
			byFrameBuff[iter]|=0x04;
			byFrameBuff[iter]|= 0x02;			
			iter++;
			//将用户地址存入帧缓冲
			__writeint_3bytes(byFrameBuff,iter,pWAAInfo->nUserId);
			int leniter = iter;
			iter+=2;
			byFrameBuff[iter++] = 0;

			BYTE   Buff[512];
			int f_iCurLen = 0;
			memset(Buff,0,sizeof(Buff));
			Buff[f_iCurLen++] = 0xA0;
			//f_iCurLen++;
			//电文长度的整字节数减1为去掉命令字1字节后的长度
			//DWORD f_byteNum = f_dwTemp/8 + (f_dwTemp%8 ? 1 : 0) - 1;
			DWORD f_byteNum=12;
			//f_iCurLen++;
			//Buff[f_iCurLen]=pPositionReport->PosType ;114.53758333333
			DWORD f_dwTemp = 0 ;
			DWORD TempBuff[10] ;
			memset(TempBuff,0,sizeof(TempBuff));
			WriteUintToBuff (TempBuff, f_dwTemp, 5, pWAAInfo->wReportHour) ;
			f_dwTemp += 5 ;
			WriteUintToBuff (TempBuff, f_dwTemp, 6, pWAAInfo->wReportMinute) ;
			f_dwTemp += 6 ;
			WriteUintToBuff (TempBuff, f_dwTemp, 6, (int)pWAAInfo->fReportSecond);
			f_dwTemp += 6 ;
			WriteUintToBuff (TempBuff, f_dwTemp, 7, (int)((pWAAInfo->fReportSecond -(int)pWAAInfo->fReportSecond)*100)) ;
			f_dwTemp += 7 ;
			WriteUintToBuff (TempBuff, f_dwTemp, 8, (int)pWAAInfo->dfLong) ;
			f_dwTemp += 8 ;
			WriteUintToBuff (TempBuff, f_dwTemp, 6, (int)((pWAAInfo->dfLong-(int)pWAAInfo->dfLong)*60));
			f_dwTemp += 6 ;
			WriteUintToBuff (TempBuff, f_dwTemp, 6, ((int)((pWAAInfo->dfLong-(int)pWAAInfo->dfLong)*3600))%60) ;
			f_dwTemp += 6 ;
			WriteUintToBuff (TempBuff, f_dwTemp, 4, ((int)((pWAAInfo->dfLong-(int)pWAAInfo->dfLong)*36000+0.5))%10) ;
			f_dwTemp += 4 ;
			WriteUintToBuff (TempBuff, f_dwTemp, 6, (int)pWAAInfo->dfLat) ;
			f_dwTemp += 6 ;
			WriteUintToBuff (TempBuff, f_dwTemp, 6, (int)((pWAAInfo->dfLat-(int)pWAAInfo->dfLat)*60)) ;
			f_dwTemp += 6 ;
			WriteUintToBuff (TempBuff, f_dwTemp, 6, ((int)((pWAAInfo->dfLat-(int)pWAAInfo->dfLat)*3600))%60) ;
			f_dwTemp += 6 ;
			WriteUintToBuff (TempBuff, f_dwTemp, 4, ((int)((pWAAInfo->dfLat-(int)pWAAInfo->dfLat)*36000+0.5))%10) ;
			f_dwTemp += 4 ;

			if(pWAAInfo->dfHgt >= 16000)////位置报告2014-01-07
			{
				WriteUintToBuff (TempBuff, f_dwTemp, 2, 1) ;
				f_dwTemp += 2 ;
				WriteUintToBuff (TempBuff, f_dwTemp, 24, (int)pWAAInfo->dfHgt) ;
				f_dwTemp += 24 ;
			}else
			{
				WriteUintToBuff (TempBuff, f_dwTemp, 2, 0) ;
				f_dwTemp += 2 ;
				if(pWAAInfo->dfHgt>= 0)
					WriteUintToBuff (TempBuff, f_dwTemp, 1, 0) ;
				else
					WriteUintToBuff (TempBuff, f_dwTemp, 1, 1) ;
				f_dwTemp +=1;

				WriteUintToBuff(TempBuff,f_dwTemp,14,abs((int)pWAAInfo->dfHgt));
				f_dwTemp+=14;

				//if(pWAAInfo-> >=0)
				WriteUintToBuff (TempBuff, f_dwTemp, 1, 0) ;
				//else
				//	WriteUintToBuff (TempBuff, f_dwTemp, 1, 1) ;
				f_dwTemp +=1;

				WriteUintToBuff(TempBuff,f_dwTemp,8,0/*abs(pWzbg->Segma)*/);
				f_dwTemp+=8;
			}

			//f_iCurLen ++;
			UINT i = 0;
			for(i = 0 ; i < f_byteNum ; i ++ )
			{
				switch(i%4)
				{
				case 0:
					Buff[f_iCurLen]=HIBYTE(HIWORD(TempBuff[i/4]));
					f_iCurLen ++ ;
					break;
				case 1:
					Buff[f_iCurLen]=LOBYTE(HIWORD(TempBuff[i/4]));
					f_iCurLen ++ ;
					break ;
				case 2:
					Buff[f_iCurLen]=HIBYTE(LOWORD(TempBuff[i/4]));
					f_iCurLen ++ ;
					break ;
				case 3:
					Buff[f_iCurLen]=LOBYTE(LOWORD(TempBuff[i/4]));
					f_iCurLen ++ ;
					break ;
				default:
					break;
				}
			}

			memcpy(&byFrameBuff[iter],Buff,f_iCurLen);
			iter += f_iCurLen;		

			__writeint_2bytes(byFrameBuff,leniter,f_iCurLen*8);

			leniter = 5;
			__writeint_2bytes(byFrameBuff,leniter,iter+1);

			byFrameBuff[iter]=Checksum(byFrameBuff,iter);
			iter++;							
			
			nBufferLen = iter;
		}
		break;

	case DEVICE_BDDWA:
		{
			int iter = 0;
			pBDDWA_STRUCT pDWAInfo=(pBDDWA_STRUCT)pMsg;
			memcpy(byFrameBuff+iter,"$DWSQ",sizeof("$DWSQ"));
			//iter+=2; //YS 2016.3.2
			iter+=5;
			iter+=2;

			__writeint_3bytes(byFrameBuff,iter,pDWAInfo->userAddr);

			byFrameBuff[iter] = 0;
			if(pDWAInfo->bifUrgency){
				byFrameBuff[iter] |= 0x20;
			}
			////0:有高程1：无高程，2：测高1，3测高2。
			if(pDWAInfo->bPosType == 1){
				byFrameBuff[iter] |= 0x04;
			}else if(pDWAInfo->bPosType == 2){
				byFrameBuff[iter] |= 0x08;
			}else if(pDWAInfo->bPosType == 3){
				byFrameBuff[iter] |= 0xC0;
			}
			
			if(pDWAInfo->bifHigh){
				byFrameBuff[iter] |= 0x01;
			}
			iter++;

			if(pDWAInfo->bPosType == 0){
				if(!pDWAInfo->bifHigh){
					if(pDWAInfo->dfHgt > 0){
						__writeint_2bytes(byFrameBuff,iter,(int)pDWAInfo->dfHgt);
					}else{
						__writeint_2bytes(byFrameBuff,iter,abs((int)pDWAInfo->dfHgt));
						iter -= 1;
						byFrameBuff[iter] |= 0x80;
						iter+=1;
					}
					__writeint_2bytes(byFrameBuff,iter,0);
				}else{
					__writeint_4bytes(byFrameBuff,iter,(int)pDWAInfo->dfHgt);
				}
			}else if(pDWAInfo->bPosType == 1){
				if(!pDWAInfo->bifHigh){
					__writeint_2bytes(byFrameBuff,iter,0);
					__writeint_2bytes(byFrameBuff,iter,(int)pDWAInfo->fAntennaHgt*10);
				}else{
					__writeint_4bytes(byFrameBuff,iter,(int)pDWAInfo->dfHgt*2);
				}
			}else if(pDWAInfo->bPosType == 2){
				if(!pDWAInfo->bifHigh){
					__writeint_2bytes(byFrameBuff,iter,0);
					__writeint_2bytes(byFrameBuff,iter,(int)pDWAInfo->fAntennaHgt*10);
				}else{
					__writeint_4bytes(byFrameBuff,iter,(int)pDWAInfo->dfHgt*2);
				}
			}else if(pDWAInfo->bPosType == 3){
				if(!pDWAInfo->bifHigh){
					
					__writeint_2bytes(byFrameBuff,iter,abs((int)pDWAInfo->dfHgt));
					iter -= 1;
					if(pDWAInfo->dfHgt < 0)
						byFrameBuff[iter] |= 0x80;
					iter += 1;

					__writeint_2bytes(byFrameBuff,iter,abs((int)pDWAInfo->fAntennaHgt));
					
				}else{
					__writeint_4bytes(byFrameBuff,iter,(int)pDWAInfo->dfHgt);
				}
			}

			if(pDWAInfo->bPosType == 0 || pDWAInfo->bPosType == 1){
				__writeint_4bytes(byFrameBuff,iter,0);
			}else{
				unsigned int data = 0;
				data = ((int)pDWAInfo->fAirPressure*10)<<12;
				data|= (int)pDWAInfo->fTemperature*10;
				__writeint_4bytes(byFrameBuff,iter,data);
			}

			__writeint_2bytes(byFrameBuff,iter,pDWAInfo->wFreq);
			int iterLen = 5;
			__writeint_2bytes(byFrameBuff,iterLen,iter+1);

			byFrameBuff[iter]=Checksum(byFrameBuff,iter);
			iter++;			

			nBufferLen = iter;
		}
		break;
	case DEVICE_GLJC:
		{
			int iter = 0;
			const int nFramelen = 12;
			lpPowerCheck pGLJCInfo=(lpPowerCheck)pMsg;
			memcpy(byFrameBuff+iter,"$GLJC",sizeof("$GLJC"));
			
			iter+=5;
			byFrameBuff[iter++] = nFramelen>>8;//frame len.
			byFrameBuff[iter++] = nFramelen&0xFF;

			iter+=2;

			__writeint_3bytes(byFrameBuff,iter,pGLJCInfo->LocalID);
			__write_byte(byFrameBuff,iter,pGLJCInfo->Freq);	

			byFrameBuff[iter]=Checksum(byFrameBuff,iter);
			nBufferLen = iter;
		}

		break;
	case DEVICE_XTZJ:
		{
			int iter = 0;
			const int nFramelen = 13;
			lpSelfCheckReq pXTZJInfo=(lpSelfCheckReq)pMsg;
			memcpy(byFrameBuff+iter,"$XTZJ",sizeof("$XTZJ"));
			iter+=5;
			byFrameBuff[iter++] = nFramelen>>8;//frame len.
			byFrameBuff[iter++] = nFramelen&0xFF;
			
			__writeint_3bytes(byFrameBuff,iter,pXTZJInfo->LocalID);
			__writeint_2bytes(byFrameBuff,iter,pXTZJInfo->freq);

			byFrameBuff[iter++]=Checksum(byFrameBuff,iter);

			nBufferLen = iter;
		}
		break;
	case DEVICE_BDLZM:
		{
			LPLZMInfo pLZMInfo = (LPLZMInfo)pMsg;
			int iter = 0;
			
			if(pLZMInfo->byManageMode == 2){
				memcpy(byFrameBuff+iter,"$LZSZ",sizeof("$LZSZ"));
				iter+=5;

				iter+=2;
				__writeint_3bytes(byFrameBuff,iter,m_uLocalID);

				int zero = (int)pLZMInfo->dfZeroValue;
				__writeint_4bytes(byFrameBuff,iter,zero);

				int pos = 5;
				__writeint_2bytes(byFrameBuff,pos,iter+1);

				byFrameBuff[iter++]=Checksum(byFrameBuff,iter);

				nBufferLen = iter;
				
			}else if(pLZMInfo->byManageMode == 1){
				memcpy(byFrameBuff+iter,"$LZDQ",sizeof("$LZDQ"));
				iter+=5;

				iter+=2;
				__writeint_3bytes(byFrameBuff,iter,m_uLocalID);

				int pos = 5;
				__writeint_2bytes(byFrameBuff,pos,iter+1);

				byFrameBuff[iter++]=Checksum(byFrameBuff,iter);

				nBufferLen = iter;				
			}
		}
		break;
	}
	return nBufferLen;
}

static BOOL DecodeICZInfo(BYTE byFrame[],int nFrameLen,lpICZInfo pICIInfo)
{

	int  iter = 10;//YS 2016.3.2
	BYTE byFrameNo = 0;

    assert((nFrameLen - 12)%3 == 0);

	byFrameNo = byFrame[iter++];
	
	pICIInfo->nUsrCntInFrm = (nFrameLen - 12)/3;
	for(int i = 0 ; i <pICIInfo->nUsrCntInFrm && i< __array_size(pICIInfo->nSubUserIds);i++){
		pICIInfo->nSubUserIds[i] = byFrame[iter++];
		pICIInfo->nSubUserIds[i] <<= 8;
		pICIInfo->nSubUserIds[i]|= byFrame[iter++];
		pICIInfo->nSubUserIds[i] <<= 8;
		pICIInfo->nSubUserIds[i]|= byFrame[iter++];

	}
	return TRUE;
}

static BOOL IsICInfo(BYTE byFrame[],int nFrameLen){
	int  iter = 10;
	return byFrame[iter] == 0;
}

static BOOL DecodeICIInfo(BYTE byFrame[],int nFrameLen,lpICIInfo pICIInfo)
{
	int  iter = 7;
	UINT uTemp;
	BYTE byTemp;

	__readint_3bytes(byFrame,iter,uTemp);
	pICIInfo->nLocalId = uTemp;//local user id;
	
	
	byTemp = byFrame[iter++];
	pICIInfo->nSerialNo = byTemp;
	if(byTemp == 0)
	{//帧号0，解析IC卡信息
		//broad id
		__readint_3bytes(byFrame,iter,uTemp);
		pICIInfo->nBroadId = uTemp;

		//user character
		__read_byte(byFrame,iter,byTemp);
		pICIInfo->nUsrAttribute = byTemp;

		//service freq
		__readint_2bytes(byFrame,iter,uTemp);
		pICIInfo->nServiceFreq = uTemp;

		//comm level
		__read_byte(byFrame,iter,byTemp);
		pICIInfo->byCommLevel = byTemp;

		//encrypt flag
		__read_byte(byFrame,iter,byTemp);
		pICIInfo->chEncriptFlag = (byTemp == 1 ? 'E': 'N');

		//sub users
		__readint_2bytes(byFrame,iter,uTemp);
		pICIInfo->nSubUsersNum = uTemp;
	}
	/*else
	{
	int k = 0;
	while(iter+3 <= nFrameLen - 1)
	{
	__readint_3bytes(byFrame,iter,uTemp);
	pICIInfo->nSubUserID[k++] = uTemp;
	}
	}*/
	return TRUE;
}

static BOOL CheckPosreport(BYTE byFrame[],int nFrameLen)
{
	assert(nFrameLen > 18);

	int  iter = 10;
	BYTE byData = byFrame[iter];

	 if(((byData&8) == 0) && (byData&0x20) && (byFrame[18] == 0xA0))
		 return TRUE;
	 else
		 return FALSE;

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

static BOOL DecodeWAAInfo(BYTE byFrame[],int nFrameLen,lpWAAInfo pWAAInfo)
{
	int iter = 7;
	UINT uTemp;
	
	pWAAInfo->byInfoType = 0 ;
	pWAAInfo->nReportFreq = 0; //

	__readint_3bytes(byFrame,iter,uTemp); //GJ 2016.3.29
	pWAAInfo->nDestionID=uTemp;    //GJ 2016.3.29

	iter+=1;
	__readint_3bytes(byFrame,iter,uTemp);
	pWAAInfo->nUserId = uTemp;
	
	int       f_iCurLen = 0;
	DWORD     f_dwTemp  = 0;

	iter = 16;
	__readint_2bytes(byFrame,iter,uTemp);//tele leng. 
	f_dwTemp = uTemp/8 + (uTemp%8 ? 1 :0);

	f_dwTemp -= 1;//0xa0;

	BYTE * pFrame = byFrame+18;

	f_iCurLen = 1;

	DWORD dwArray[200] ;
	memset(dwArray,0,sizeof(dwArray));

	int icounter;
	if(f_dwTemp%4!=0)
		icounter = f_dwTemp/4 +1;
	else
		icounter = f_dwTemp/4;

	int i = 0;
	for(; i < icounter;i++ )
	{
		dwArray[i] = pFrame[f_iCurLen+i*4] ;
		dwArray[i] *= 256 ;
		dwArray[i] += pFrame[f_iCurLen+i*4+1] ;
		dwArray[i] *= 256 ;
		dwArray[i] += pFrame[f_iCurLen+i*4+2] ;
		dwArray[i] *= 256 ;
		dwArray[i] += pFrame[f_iCurLen+i*4+3] ;
	}
	//tuju
	//读取时间
	i=0 ;
	pWAAInfo->wReportHour = GetUintFromBuff(dwArray, i, 5) ;//5
	i+= 5 ;
	pWAAInfo->wReportMinute  = GetUintFromBuff(dwArray, i, 6) ;//6
	i+= 6 ;
	pWAAInfo->fReportSecond = GetUintFromBuff(dwArray, i ,6);//6
	i+=6;
	pWAAInfo->fReportSecond  += GetUintFromBuff(dwArray, i ,7)*0.01;//7
	i+=7;
	//经度
	pWAAInfo->dfLong = GetUintFromBuff(dwArray, i ,8);//8
	i+=8;
	pWAAInfo->dfLong += GetUintFromBuff(dwArray, i ,6)/60.0;//6
	i+=6;
	pWAAInfo->dfLong += GetUintFromBuff(dwArray, i ,6)/3600.0;
	i+=6;
	pWAAInfo->dfLong += GetUintFromBuff(dwArray, i ,4)/36000.0;
	i+=4;
	//纬度
	pWAAInfo->dfLat= GetUintFromBuff(dwArray, i ,6);//6
	i+=6;
	pWAAInfo->dfLat+= GetUintFromBuff(dwArray, i ,6)/60.0;//6
	i+=6;
	pWAAInfo->dfLat+= GetUintFromBuff(dwArray, i ,6)/3600.0;//6
	i+=6;
	pWAAInfo->dfLat+= GetUintFromBuff(dwArray, i ,4)/36000.0;//4
	i+=4;
	
	i+=2;
	DWORD dwData =  GetUintFromBuff(dwArray, i ,24);
	i+=24;

	i-=24;
	int nSign = GetUintFromBuff(dwArray, i ,1);
	i+=1;
	if(nSign)
	{
		pWAAInfo->dfHgt = GetUintFromBuff(dwArray, i ,14) *-1;//14
	}
	else
	{
		pWAAInfo->dfHgt = GetUintFromBuff(dwArray, i ,14);
	}
	i += 14;	

	pWAAInfo->chHgtUnit = 'M';

	return TRUE;
}

static BOOL DecodeTXRInfo(BYTE byFrame[],int nFrameLen,lpTXRInfo pTXRInfo)
{
	int  iter = 7;
	UINT uTemp;
	BYTE byTemp;

	//发信方地址
	__readint_3bytes(byFrame,iter,uTemp);
	pTXRInfo->nDestionID = uTemp;

	__read_byte(byFrame,iter,byTemp);
	pTXRInfo->byInfoType = byTemp&8 ? 4 : 1;

	pTXRInfo->byTeleFormat = byTemp&0x20 ? 1 : 0;
	
	__readint_3bytes(byFrame,iter,uTemp);
	pTXRInfo->nLocalID = uTemp;

	__read_byte(byFrame,iter,byTemp);
	pTXRInfo->bySendHour = byTemp;

	__read_byte(byFrame,iter,byTemp);
	pTXRInfo->bySendMinute = byTemp;

	__readint_2bytes(byFrame,iter,uTemp);
	
	BYTE flag;
	__read_byte(byFrame,iter,flag);
	iter--;
	if(pTXRInfo->byTeleFormat == 1){//bcd
		if(flag != 0xA0 && flag!= 0xA4)
		{//bcd
			int chars = bytes_hexchars(pTXRInfo->chContent,sizeof(pTXRInfo->chContent),&byFrame[iter],uTemp);
			pTXRInfo->nContentLen = chars;
			pTXRInfo->chContent[chars] = '\0';
		}else if(flag == 0xA0){
			return FALSE;
		}else if(flag == 0xA4){
			//混发通信
			assert(uTemp%8 == 0);	
			pTXRInfo->byTeleFormat = 2;
			memcpy_s(pTXRInfo->chContent,sizeof(pTXRInfo->chContent),&byFrame[iter+1],uTemp/8 -1);
			pTXRInfo->nContentLen = uTemp/8 -1;
			pTXRInfo->chContent[pTXRInfo->nContentLen] = '\0';
		}
		
	}else{
		//汉字通信
		assert(uTemp%8 == 0);			
		memcpy_s(pTXRInfo->chContent,sizeof(pTXRInfo->chContent),&byFrame[iter],uTemp/8);
		pTXRInfo->nContentLen = uTemp/8;
		pTXRInfo->chContent[pTXRInfo->nContentLen] = '\0';
	}

	iter+=uTemp/8;

	__read_byte(byFrame,iter,byTemp);
	pTXRInfo->byCRC=byTemp;

	return TRUE;
}

static BOOL DecodeDWRInfo(BYTE byFrame[],int nFrameLen,lpDWRInfo pDWRInfo)
{
	int  iter = 7;
	UINT uTemp;
	BYTE byTemp;

	__readint_3bytes(byFrame,iter,uTemp);
	pDWRInfo->nLocalId = uTemp;//首先赋值为本机地址

	__read_byte(byFrame,iter,byTemp);

	pDWRInfo->nPosType = ((byTemp & 0x20) == 0) ? 1 : 2;
	pDWRInfo->byPrecisionInd = ((byTemp&0x08) == 0) ? 0:1;
	pDWRInfo->bUrgent = ((byTemp&0x04) == 0) ? FALSE : TRUE;
	pDWRInfo->bMultiValue = ((byTemp&0x02) == 0) ? FALSE : TRUE;
	pDWRInfo->bGaoKong = (byTemp & 0x01) ? TRUE:FALSE;

	if(pDWRInfo->nPosType == 2){//定位查询,更改nLocalId
		__readint_3bytes(byFrame,iter,uTemp);
		pDWRInfo->nLocalId = uTemp;
	}else//YS 2016.3.2
		iter += 3;

	__read_byte(byFrame,iter,byTemp);//time
	pDWRInfo->nPosHour = byTemp;

	__read_byte(byFrame,iter,byTemp);
	pDWRInfo->nPosMinute = byTemp;

	__read_byte(byFrame,iter,byTemp);
	pDWRInfo->fPosSecond = byTemp;

	__read_byte(byFrame,iter,byTemp);
	pDWRInfo->fPosSecond+= byTemp*0.01;

	__read_byte(byFrame,iter,byTemp);//long
	pDWRInfo->dfLong = byTemp;

	__read_byte(byFrame,iter,byTemp);
	pDWRInfo->dfLong += byTemp/60.0;

	__read_byte(byFrame,iter,byTemp);
	pDWRInfo->dfLong += byTemp/3600.0;

	__read_byte(byFrame,iter,byTemp);
	pDWRInfo->dfLong += byTemp/36000.0;

	__read_byte(byFrame,iter,byTemp);//lat
	pDWRInfo->dfLat = byTemp;

	__read_byte(byFrame,iter,byTemp);
	pDWRInfo->dfLat += byTemp/60.0;

	__read_byte(byFrame,iter,byTemp);
	pDWRInfo->dfLat += byTemp/3600.0;

	__read_byte(byFrame,iter,byTemp);
	pDWRInfo->dfLat += byTemp/36000.0;

	__readint_2bytes(byFrame,iter,uTemp);

	int nSignFlag = uTemp & 0x4000;
	if(nSignFlag){
		pDWRInfo->fEarthHgt = uTemp& 0x3FFF;
		pDWRInfo->fEarthHgt *= -1;
	}else{
		pDWRInfo->fEarthHgt = uTemp& 0x3FFF;
	}

	__readint_2bytes(byFrame,iter,uTemp);
	nSignFlag = uTemp & 0x0100;

	if(nSignFlag){
		pDWRInfo->dfHgtOut = uTemp&0xFF;
		pDWRInfo->dfHgtOut *= -1;
	}else{
		pDWRInfo->dfHgtOut = uTemp&0xFF;
	}

	pDWRInfo->chLatDirection = 'N';
	pDWRInfo->chLongDirection = 'E';

	return TRUE;
}

static bool DecodeFKIinfo(BYTE byFrame[],int nFrameLen,lpFKIInfo pFKIInfo)
{
	int iter=10;
	UINT uTemp;
	BYTE byTemp;

	memset(pFKIInfo,0,sizeof(FKIInfo));

	pFKIInfo->chExeRslt = 'N';
	__read_byte(byFrame,iter,byTemp);
	if(byTemp == 0x00)
	{
		pFKIInfo->chExeRslt = 'Y';		
		memcpy_s(pFKIInfo->chCommandName,sizeof(pFKIInfo->chCommandName),byFrame+iter,4);
	}else if(byTemp==0x04)
	{
		__readint_2bytes(byFrame,iter,uTemp);
		pFKIInfo->byMinuteWait=uTemp;
		__readint_2bytes(byFrame,iter,uTemp);
		pFKIInfo->bySecondWait=uTemp;
	}else if(byTemp==0x03)
	{
		pFKIInfo->byTranInhabitInd = 2;
	}else if(byTemp==0x07)
	{
		pFKIInfo->byTranInhabitInd = 1;
	}else if(byTemp==0x08)
	{
		pFKIInfo->byTranInhabitInd = 3;
	}

	return TRUE;
}
static bool DecodeGLZKInfo(BYTE byFrame[],int nFrameLen,lpPowerStatus pGLZKInfo)
{
	int  iter = 7;
	UINT uTemp;
	BYTE byTemp;
	__readint_3bytes(byFrame,iter,uTemp);
	pGLZKInfo->LocalID = uTemp;//首先赋值为本机地址

	__read_byte(byFrame,iter,byTemp);
	pGLZKInfo->Power1=byTemp;

	__read_byte(byFrame,iter,byTemp);
	pGLZKInfo->Power2=byTemp;

	__read_byte(byFrame,iter,byTemp);
	pGLZKInfo->Power3=byTemp;

	__read_byte(byFrame,iter,byTemp);
	pGLZKInfo->Power4=byTemp;

	__read_byte(byFrame,iter,byTemp);
	pGLZKInfo->Power5=byTemp;

	__read_byte(byFrame,iter,byTemp);
	pGLZKInfo->Power6=byTemp;

	return TRUE;
}

static bool DecodeZJXXInfo(BYTE byFrame[],int nFrameLen,lpSelfCheckInfo pZJXXInfo)
{
		int iter=7;
		UINT uTemp;
		BYTE byTemp;
		__readint_3bytes(byFrame,iter,uTemp);
		pZJXXInfo->LocalID = uTemp;//首先赋值为本机地址

		__read_byte(byFrame,iter,byTemp);
		pZJXXInfo->ICCardState=byTemp;

		__read_byte(byFrame,iter,byTemp);
		pZJXXInfo->HardState=byTemp;  

		__read_byte(byFrame,iter,byTemp);
		pZJXXInfo->Batterystate=byTemp;

		__read_byte(byFrame,iter,byTemp);
		pZJXXInfo->InState=byTemp;

		for(int i=0;i<6;i++)
		{
			__read_byte(byFrame,iter,byTemp);
			pZJXXInfo->PowerState[i]=byTemp;
		}
		return TRUE;
}
static bool DecodeZLXYInfo(BYTE byFrame[],int nFrameLen,lpZLXYInfo pZLXYInfo)
{
	int iter=2;
	UINT uTemp;
	BYTE byTemp;
	__readint_2bytes(byFrame,iter,uTemp);

	pZLXYInfo->nLatDegree=(uTemp>>4)&0x0FFF;
	pZLXYInfo->nLatMinute=(uTemp&0x000F)*10;
	__readint_2bytes(byFrame,iter,uTemp);
	pZLXYInfo->nLatMinute+=(uTemp>>12)&0x000F;
	pZLXYInfo->fLatSecond=(uTemp&0x0FFF)*0.1;

	__read_byte(byFrame,iter,byTemp);
	pZLXYInfo->nLongDegree=byTemp;	
	__read_byte(byFrame,iter,byTemp);
	pZLXYInfo->nLongMinute=byTemp;
	__readint_2bytes(byFrame,iter,uTemp);
	pZLXYInfo->fLongSecond=(uTemp>>8)*0.1;

	pZLXYInfo->byOrient=uTemp&0x0004?'D1':'D0';
	__read_byte(byFrame,iter,byTemp);

	pZLXYInfo->nBatteryCap=byTemp;

	return TRUE;
}
static bool DecodeZTSBInfo(BYTE byFrame[],int nFrameLen,lpUZTSBInfo pZTSBInfo)
{
	int iter=2;
	BYTE byTemp;
	UINT uTemp;
	__read_byte(byFrame,iter,byTemp);
	pZTSBInfo->nRepFreq=byTemp;

	__readint_2bytes(byFrame,iter,uTemp);

	pZTSBInfo->nLatDegree=(uTemp>>4)&0x0FFF;
	pZTSBInfo->nLatMinute=(uTemp&0x000F)*10;
	__readint_2bytes(byFrame,iter,uTemp);
	pZTSBInfo->nLatMinute+=(uTemp>>12)&0x000F;
	pZTSBInfo->fLatSecond=(uTemp&0x0FFF)*0.1;

	__read_byte(byFrame,iter,byTemp);
	pZTSBInfo->nLongDegree=byTemp;	
	__read_byte(byFrame,iter,byTemp);
	pZTSBInfo->nLongMinute=byTemp;
	__readint_2bytes(byFrame,iter,uTemp);
	pZTSBInfo->fLongSecond=(uTemp>>8)*0.1;

	pZTSBInfo->byOrient=uTemp&0x0004?'D1':'D0';
	__read_byte(byFrame,iter,byTemp);

	pZTSBInfo->nBatteryCap=byTemp;
	return true;

}

static bool DecodeSBXInfo(BYTE byFrame[],int nFrameLen,lpSBXInfo pSBXInfo)
{
	int iter = 5;
	int len = 0;
	int i = 0 ;

	memset(pSBXInfo,0,sizeof(SBXInfo));

	__readint_2bytes(byFrame,iter,len);
	int verlen = len - 11;
	if(verlen > sizeof(pSBXInfo->chSoftVer)-1)
		verlen = sizeof(pSBXInfo->chSoftVer)-1;

	for(; i < verlen ; i++){
		pSBXInfo->chSoftVer[i] = byFrame[10+i];
	}

	pSBXInfo->chSoftVer[i] = '\0';

	return true;
}

static bool DecodeGXMInfo(BYTE byFrame[],int nFrameLen,lpGXMInfo pGXMInfo)
{
	int iter = 10;
	
	pGXMInfo->nSerialNo = 0;
	pGXMInfo->nType = 3;

	int bytes = HexToAsc(pGXMInfo->chMagInfo,sizeof(pGXMInfo->chMagInfo),byFrame+iter,256);
	pGXMInfo->chMagInfo[bytes] = '\0';

	return true;

}

static bool DecodeLZMInfo(BYTE byFrame[],int nFrameLen,LPLZMInfo pLZMInfo)
{
	int iter = 10;
	UINT uTemp = 0;	

	__readint_2bytes(byFrame,iter,uTemp);
	pLZMInfo->dfZeroValue = uTemp;
	pLZMInfo->byManageMode = 3;

	return true;
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

int  CRdss4_0SearchExp::GetAccumulateCnt(UINT uMsgId)
{
	int nRslt = 0;

	/*if(m_mapProtCnt.count(uMsgId) > 0)
	nRslt = m_mapProtCnt[uMsgId];
	*/
	return nRslt;
}

BYTE CRdss4_0SearchExp::excuteplugin(BYTE byFrame[] , int nFrameLen,BYTE *DataBuff, DWORD &Len)
{
	AUTOLOCK(g_lock_plugin);

	BYTE byRtn = CUNPACK_NULL;
	char chTitle[6];

	assert(DataBuff != NULL);
	assert(byFrame != NULL && nFrameLen>0);

	if(DataBuff == NULL || byFrame == NULL || nFrameLen <1) return CUNPACK_NULL;

	memcpy(chTitle,byFrame,5);
	chTitle[5] = '\0';

	size_t i = 0;
	for(; i < m_plugins.size();i++){
		if(strcmp(chTitle,m_plugins[i].title) == 0)
			break;
	}

	if(i == m_plugins.size()) return CUNPACK_NULL;

	if(m_plugins[i].pExpalin){
		int msgid = m_plugins[i].pExpalin((char*)byFrame,nFrameLen,DataBuff,Len);
		return msgid;
	}else
		return CUNPACK_NULL;
}

BYTE CRdss4_0SearchExp::DataExplain(BYTE byFrame[] , int nFrameLen,BYTE *DataBuff, DWORD &Len)
{
	BYTE byRtn = CUNPACK_NULL;
	int  nTitleType = CUNPACK_NULL;
	char chTitle[6];
	BOOL bExplain = FALSE;

	assert(DataBuff != NULL);
	assert(byFrame != NULL && nFrameLen>0);

	if(DataBuff == NULL || byFrame == NULL || nFrameLen <1) return 0;
	
	memcpy(chTitle,byFrame,5);
	chTitle[5] = '\0';

	nTitleType = CheckTitleType(chTitle);
	if(nTitleType == CUNPACK_NULL) return CUNPACK_NULL;

	switch(nTitleType)
	{
	case CUNPACK_HZR:
		{
			int sender;
			unsigned char u8data;

			lpHZRInfo pHZRInfo = (lpHZRInfo)DataBuff;
			memset(pHZRInfo,0,sizeof(HZRInfo));

			int iter = 10;
			__readint_3bytes(byFrame,iter,sender);

			pHZRInfo->nLocalId = sender;

			__read_byte(byFrame,iter,u8data);
			pHZRInfo->byResponseNum = u8data;

			for(size_t i = 0 ; i < u8data && i < 5; i++){
				__read_byte(byFrame,iter,u8data);
				pHZRInfo->wSendHour[i] = u8data;
				__read_byte(byFrame,iter,u8data);
				pHZRInfo->wSendMinute[i] = u8data;

				__read_byte(byFrame,iter,u8data);
				pHZRInfo->wAckHour[i] = u8data;
				__read_byte(byFrame,iter,u8data);
				pHZRInfo->wAckMinute[i] = u8data;
			}
			byRtn=CUNPACK_HZR;  //GJ 2016.3.29
		}
		break;
	case CUNPACK_ZHR:
		{
			ZHRInfo * pInfo = (ZHRInfo*)DataBuff;
			memset(pInfo,0,sizeof(ZHRInfo));
			int iter = 10;

			int count = HexToAsc(pInfo->ZHInfo,sizeof(pInfo->ZHInfo),byFrame+iter,32);
			pInfo->ZHInfo[count] = '\0';

			byRtn=CUNPACK_ZHR;
		}
		break;
	case CUNPACK_SBX:
		{
			lpSBXInfo pBSXInfo = (lpSBXInfo)DataBuff;
			bExplain = DecodeSBXInfo(byFrame,nFrameLen,pBSXInfo);
			if(bExplain){
				byRtn=CUNPACK_SBX;
			}
		}
		break;
	case CUNPACK_GXM:
		{
			lpGXMInfo pGXMInfo = (lpGXMInfo)DataBuff;
			bExplain = DecodeGXMInfo(byFrame,nFrameLen,pGXMInfo);
			if(bExplain){
				byRtn=CUNPACK_GXM;
			}
		}
		break;
	case CUNPACK_LZM:
		{
			LPLZMInfo pLZMInfo = (LPLZMInfo)DataBuff;
			bExplain = DecodeLZMInfo(byFrame,nFrameLen,pLZMInfo);
			if(bExplain){
				byRtn=CUNPACK_LZM;
			}
		}
		break;
	case CUNPACK_ZDA:
		{
			lpZDAInfo pZDAInfo=(lpZDAInfo)DataBuff;
			bExplain=DecodeZDAInfo(byFrame,nFrameLen,pZDAInfo);
			if(bExplain){
				byRtn=CUNPACK_ZDA;
			}
		}
		break;
	case CUNPACK_GGA:
		{
			LPGGAInfo pGGA = (LPGGAInfo)DataBuff;
			memset(pGGA,0,sizeof(GGAInfo));

			bExplain = DecodeGGAInfo(byFrame,nFrameLen,pGGA);
			if(bExplain){				
				byRtn = CUNPACK_GGA;
				memcpy(&g_ggaPos,pGGA,sizeof(GGAInfo));
			}
		}
		break;
	case CUNPACK_ICI:
		{
			static int TotalUserCount = 0;
			
			if(IsICInfo(byFrame,nFrameLen)){
				lpICIInfo pICIInfo = (lpICIInfo)DataBuff;
				bExplain = DecodeICIInfo(byFrame,nFrameLen,pICIInfo);

				if(bExplain){
					TotalUserCount = pICIInfo->nSubUsersNum;
					m_uLocalID = pICIInfo->nLocalId;
					byRtn = CUNPACK_ICI;
					memcpy(m_ici,pICIInfo,sizeof(ICIInfo));
				}else{
					TotalUserCount = 0;
				}
			}else{
				if(TotalUserCount == 0) return CUNPACK_NULL;
				
				lpICZInfo pICZInfo = (lpICZInfo)DataBuff;
				bExplain = DecodeICZInfo(byFrame,nFrameLen,pICZInfo);

				if(bExplain){
					pICZInfo->nTotalSubUserNums = TotalUserCount;
					byRtn = CUNPACK_ICZ;
				}				
			}
		}
		break;

	case CUNPACK_TXR:
		{
			if(CheckPosreport(byFrame,nFrameLen)){
				//位置报告
				lpWAAInfo pWAAInfo = (lpWAAInfo)DataBuff;
				bExplain = DecodeWAAInfo(byFrame,nFrameLen,pWAAInfo);
				
				if(bExplain){
					byRtn = CUNPACK_WAA;
				}
				
			}else{//通信
				lpTXRInfo pTXRInfo = (lpTXRInfo)DataBuff;
				bExplain = DecodeTXRInfo(byFrame,nFrameLen, pTXRInfo);

				if(bExplain){
					byRtn = CUNPACK_TXR;
				}				
			}			
		}
		break;

	case CUNPACK_DWR:
		{
			lpDWRInfo pDWRInfo = (lpDWRInfo)DataBuff;
			bExplain = DecodeDWRInfo(byFrame,nFrameLen, pDWRInfo);

			if(bExplain){
				byRtn = CUNPACK_DWR;
			}	
		}
		break;

	//反馈信息
	case CUNPACK_FKI:
		{
			lpFKIInfo pFKIInfo=(lpFKIInfo)DataBuff;
			bExplain=DecodeFKIinfo(byFrame,nFrameLen,pFKIInfo);
			if(bExplain)
			{
				byRtn=CUNPACK_FKI;
			}
		}
		break;
	case CUNPACK_BSI:
		{
			lpPowerStatus pGLZKInfo=(lpPowerStatus)DataBuff;
			bExplain=DecodeGLZKInfo(byFrame,nFrameLen,pGLZKInfo);
			if(bExplain)
			{
				BeamInfo beam;
				memset(&beam,0,sizeof(beam));
				
				beam.byPower[0] = pGLZKInfo->Power1;
				beam.byPower[1] = pGLZKInfo->Power2;
				beam.byPower[2] = pGLZKInfo->Power3;
				beam.byPower[3] = pGLZKInfo->Power4;
				beam.byPower[4] = pGLZKInfo->Power5;
				beam.byPower[5] = pGLZKInfo->Power6;
				
				memcpy(DataBuff,&beam,sizeof(beam));

				byRtn=CUNPACK_BSI;
			}
		}
		break;
	case CUNPACK_ZTI:
		{
			lpSelfCheckInfo pZJXXInfo=(lpSelfCheckInfo)DataBuff;
			bExplain=DecodeZJXXInfo(byFrame,nFrameLen,pZJXXInfo);
			if(bExplain)
			{
				ZTIInfo zti;
				memset(&zti,0,sizeof(zti));				
				memcpy(&(zti.data.selfcheck),pZJXXInfo,sizeof(SelfCheckInfo));
				BeamInfo beam;
				memset(&beam,0,sizeof(beam));
				for(int i = 0 ; i < 6 ; i++)
					beam.byPower[i] = pZJXXInfo->PowerState[i];
				lpZTIInfo ztiptr = (lpZTIInfo)DataBuff;
				memcpy(ztiptr,&zti,sizeof(ZTIInfo));
				memcpy(DataBuff + sizeof(ZTIInfo),&beam,sizeof(beam));
				byRtn=CUNPACK_ZTI;
			}
		}
		break;	
	}

	return byRtn;

}