#ifndef __RDSS_40_SEARCHEXP_BONA__H
#define __RDSS_40_SEARCHEXP_BONA__H

#include <vector>
#include <map>
#include "Rdss4_0SearchExp.h"

#include "protocal/ChnavtectGlobal.h"
#include "Struct.h"
#include "../Head.h"
using namespace std;

class CRdss4_0SearchExp_BONA: public CRdss4_0SearchExp
{
public:
	CRdss4_0SearchExp_BONA();
	virtual ~CRdss4_0SearchExp_BONA();
	virtual void Release(){delete this;}

protected:
	virtual int explain_gga(BYTE byFrame[] , int nFrameLen,BYTE *DataBuff, DWORD &Len);
	virtual int explain_zda(BYTE byFrame[] , int nFrameLen,BYTE *DataBuff, DWORD &Len);
	void SetRdssTitles();
};

#endif