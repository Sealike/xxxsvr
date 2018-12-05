#include "findjobber.h"
#include "util/util.h"

sess_t getrefsvr(const char* cname)
{
	sess_t rtn = (sess_t)xhash_get(client()->sessions,cname);

	return rtn;
}

sess_t getrefclnt(const char* cname)
{
	sess_t rtn = (sess_t)xhash_get(g_instapp->sessions, cname);

	return rtn;
}