#ifndef _JSON_STRING_H
#define _JSON_STRING_H



#ifdef __cplusplus
extern "C"
{
#endif

#include "util/util.h"
#include "bdjobber_callback.h"

/* λ���ϱ� */
typedef struct _bdposreport_st{
	int bdid;		// ���û��ı�����
	char *taskid;
	char *executor;
	double longtitue;
	double lattitude;
	float height;		// �߶ȣ�
	char time[64];
}bdposreport_st, *bdposreport_t;

/* �¼��ϱ� */
typedef struct _evtreport_st{
	
	int bdid;		// ���û��ı�����
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

/* ������·״̬��Ϣ�ϱ� */
typedef struct _bsireport_st{
	int bdid;		// ָ�ӻ�����ı�����
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