#include "json_string.h"


//char* json_string(void* arg, int type)
//{
//	char  * rslt = NULL;
//
//	cJSON * root = cJSON_CreateObject();
//
//	switch (type)
//	{
//	case  DYNCODE:
//	{
//		dyncode_t dyncode = (dyncode_t)arg;
//
//		cJSON_AddStringToObject(root, "taskid", dyncode->taskid);
//		cJSON_AddStringToObject(root, "net", dyncode->net);
//		cJSON_AddStringToObject(root, "executor", dyncode->executor);
//		cJSON_AddStringToObject(root, "rslt", dyncode->rslt);
//
//	}
//	break;
//
//	case CONFIGACK:
//	{
//		configack_t configack = (configack_t)arg;
//
//		cJSON_AddStringToObject(root, "rslt", configack->rslt);
//		cJSON_AddStringToObject(root, "err", configack->err);
//		cJSON_AddStringToObject(root, "seqno", configack->seqno);
//	}
//	break;
//
//	default:
//		break;
//	}
//
//
//
//
//	rslt = cJSON_PrintUnformatted(root);
//
//	cJSON_Delete(root);
//	return rslt;
//}
