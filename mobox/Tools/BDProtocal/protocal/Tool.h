#ifndef __TOOL_H_
#define __TOOL_H_

template<typename T>
class CToolClass
{
public:
	CToolClass(void){;}
	~CToolClass(void){;}
};

static int     g_CommCapacity[2][4] = {{140,360,580,1680},{110,408,628,848}};
static TCHAR  *g_strAttr[8] = {_T("ָ�����û���"),_T("һ���û���"),_T("�����û���"),_T("�����û���"),_T("ָ�����û���(���������֤)"),_T("һ���û���(���������֤)"),_T("�����û���(���������֤)"),_T("�����û���(���������֤)")};
template<>
class CToolClass<lpICIInfo>
{
public: 
	CToolClass(lpICIInfo pInfo){m_pICIInfo = pInfo;}
	~CToolClass(){;}
	
	lpICIInfo GetInfo(){return m_pICIInfo;}
	wstring GetAttribute()
	{	
		if(m_pICIInfo->nUsrAttribute >= 0 && m_pICIInfo->nUsrAttribute < 8)
			return g_strAttr[m_pICIInfo->nUsrAttribute];
		else
			return _T("δ֪����");
	}
    
	wstring GetEncyptString()
	{   
		if(m_pICIInfo->chEncriptFlag == 'E')
			return _T("����");
		else if(m_pICIInfo->chEncriptFlag == 'N')
			return _T("����");
		else 
			return _T("δ֪����");
	}   
	
	int GetCommBitsLen(BOOL bUrgent)
	{	
		int nLen = 0;
		if(!bUrgent)
		{   
			if(m_pICIInfo->byCommLevel >0 && m_pICIInfo->byCommLevel < 5)
			{
				if(m_pICIInfo->chEncriptFlag == 'E')
				{	
					nLen = g_CommCapacity[0][m_pICIInfo->byCommLevel -1];
				}else
					nLen = g_CommCapacity[1][m_pICIInfo->byCommLevel -1];
			}			
		}else
			nLen =  188;	
		
		return nLen;
	}
private:
	ICIInfo* m_pICIInfo;
	
};

#endif