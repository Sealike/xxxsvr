#ifndef __TYPE_H__
#define __TYPE_H__


class ILocker;
#include "mysqldb.h"
class CSharedRes
{
public:
	virtual ILocker*  get_reslock()    = NULL;
	virtual void*     get_queryhandle()= NULL;
private:
};


#endif