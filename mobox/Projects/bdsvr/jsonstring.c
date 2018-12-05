#include "jsonstring.h"
#include "json/cJSON.h"
#include "subst/subst.h"
#include "expat.h"
#include "instapp.h"

char * sz_evttype[] = {
	"",
	"�ҿ��ķ�",
	"��ϡ����",
	"��������",
	"�溦",
	"����"
};

extern instapp_t g_instapp;

// �ϱ�λ����Ϣ��json����ƴ������
char* posreport_jsonstring(bdposreport_t bdposreport)
{
	char  * rslt = NULL;
	char    szvalue[64];
	cJSON * root = cJSON_CreateObject();

	//{ "type":"bdposreport", "userid" : "23424", " lon" : "114.234234", "lat" : "38.23424244", "seqno" : "231" }

	//{"taskid":"t129","executor":"liuzhizhi",��net��:��bd/inet��,��data��:[{��time��:��2018-06-06 12:12:12��,��lon��:��114.7464643��,��lat��:��32.8364555��,��hgt��:��65��}]}*xx\r\n
	
	cJSON_AddStringToObject(root, "taskid", bdposreport->taskid);

	snprintf(szvalue, sizeof(szvalue), "%d", bdposreport->bdid);
	cJSON_AddStringToObject(root, "executor", szvalue);

	snprintf(szvalue, sizeof(szvalue), "%.8f", bdposreport->longtitue);
	cJSON_AddStringToObject(root, "lon", szvalue);

	snprintf(szvalue, sizeof(szvalue), "%.8f", bdposreport->lattitude);
	cJSON_AddStringToObject(root, "lat", szvalue);

	snprintf(szvalue, sizeof(szvalue), "%f", bdposreport->height);
	cJSON_AddStringToObject(root, "hgt", szvalue);

	cJSON_AddStringToObject(root, "time", bdposreport->time);

	snprintf(szvalue, sizeof(szvalue), "%d", g_instapp->seqno2frontsvr);
	cJSON_AddStringToObject(root, "seqno", szvalue);

	rslt = cJSON_PrintUnformatted(root);

	cJSON_Delete(root);
	return rslt;
}


// �ϱ��¼���json����ƴ������
char* evtreport_jsonstring(evtreport_t evtreport)
{
	char  * rslt = NULL;
	char    szvalue[64];
	cJSON * root = cJSON_CreateObject();

	//{"type":"evtreport","userid":"23424"," evttype":"23","desc":"�ҿ��ķ�",
	// "time":"xxxx-xx-xx xx:xx:xx","lat":"xx.xxxxxx","lon":"xxx.xxxxxxx","hgt":"xxx","seqno":"231"}

	cJSON_AddStringToObject(root, "type", "bdevtreport");

	snprintf(szvalue, sizeof(szvalue), "%d", evtreport->bdid);
	cJSON_AddStringToObject(root, "bdid", szvalue);

	snprintf(szvalue, sizeof(szvalue), "%d", evtreport->evt_type);
	cJSON_AddStringToObject(root, "evttype", szvalue);

	cJSON_AddStringToObject(root, "evttypename", sz_evttype[evtreport->evt_type]);

	cJSON_AddStringToObject(root, "desc", evtreport->content);

	snprintf(szvalue, sizeof(szvalue), "%.8f", evtreport->longtitue);
	cJSON_AddStringToObject(root, "lon", szvalue);

	snprintf(szvalue, sizeof(szvalue), "%.8f", evtreport->lattitude);
	cJSON_AddStringToObject(root, "lat", szvalue);

	snprintf(szvalue, sizeof(szvalue), "%f", evtreport->height);
	cJSON_AddStringToObject(root, "hgt", szvalue);

	cJSON_AddStringToObject(root, "time", evtreport->time);

	snprintf(szvalue, sizeof(szvalue), "%d", g_instapp->seqno2frontsvr);
	cJSON_AddStringToObject(root, "seqno", szvalue);

	rslt = cJSON_PrintUnformatted(root);

	cJSON_Delete(root);
	return rslt;
}


// �ϱ�������·״̬��json����ƴ������
char* bsireport_jsonstring(bsireport_t bsireport)
{
	char  * rslt = NULL;
	char    szvalue[64];
	cJSON * root = cJSON_CreateObject();

	//{"type":"bdbsi", "bdid" : "23424", " state" : [2, 3, 4, 2, 2, 3, 8, 4, 4, 3], "time" : "xxxx-xx-xx xx:xx:xx", "seqno" : "231"}

	cJSON_AddStringToObject(root, "type", "bdbsi");

	snprintf(szvalue, sizeof(szvalue), "%d", bsireport->bdid);	
	cJSON_AddStringToObject(root, "bdid", szvalue);

	cJSON_AddStringToObject(root, "state", bsireport->state);

	cJSON_AddStringToObject(root, "time", bsireport->time);

	snprintf(szvalue, sizeof(szvalue), "%d", g_instapp->seqno2frontsvr);
	cJSON_AddStringToObject(root, "seqno", szvalue);

	rslt = cJSON_PrintUnformatted(root);

	cJSON_Delete(root);
	return rslt;
}




