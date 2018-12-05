#include "getrefsvr.h"


sess_t getrefsvr(const char* cname)
{
	if (client()->sessions == NULL) return NULL;
	sess_t rtn = (sess_t)xhash_get(client()->sessions, cname);

	return rtn;
}

int sess_rename(sess_t sess, const char * newname,int len)
{

	// step 1. 更新哈希表
	if (client()->sessions == NULL)
	{
		return -1;
	}
	char * name = _strdup(newname);
	xhash_zap(client()->sessions, sess->sname);
	xhash_put(client()->sessions, name, sess);


	// step 2. 更换sess名称
	memcpy(sess->sname, newname, len);	

	return 0;
}