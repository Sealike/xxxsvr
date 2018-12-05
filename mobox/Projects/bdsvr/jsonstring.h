#ifndef _JSON_STRING_H
#define _JSON_STRING_H



#ifdef __cplusplus
extern "C"
{
#endif

#include "util/util.h"
#include "bdjobber_callback.h"

/* 位置上报 */
typedef struct _bdposreport_st{
	int bdid;		// 子用户的北斗号
	char *taskid;
	char *executor;
	double longtitue;
	double lattitude;
	float height;		// 高度？
	char time[64];
}bdposreport_st, *bdposreport_t;

/* 事件上报 */
typedef struct _evtreport_st{
	
	int bdid;		// 子用户的北斗号
	char *taskid;
	char *executor;
	char *time;
	char *digest;
	char * hid;
	double longtitue;
	double lattitude;
	float height;
	bd_evt_type_t evt_type;
	char *content;
}evtreport_st, *evtreport_t;

/* 数据链路状态信息上报 */
typedef struct _bsireport_st{
	int bdid;		// 指挥机本身的北斗号
	char state[30];
	char time[64];
}bsireport_st, *bsireport_t;



char* evtreport_jsonstring(evtreport_t evtreport);
char* posreport_jsonstring(bdposreport_t bdposreport);
char* bsireport_jsonstring(bsireport_t bsireport);

#ifdef __cplusplus
}
#endif



#endif