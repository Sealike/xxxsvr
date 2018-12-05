//#include "p2pclient.h"
//#include "protobuffer.h"
//#include "router_expat.h"
//#include "client.h"
//#include "rediscommand.h"
//#include "json/cJSON.h"
//#include "dbaccess.h"
//#include "routermsgcallback.h"
//
//extern redisContext* g_rediscontext;
//static char g_msgcache[64 * 1024];
//static unsigned char checksum(unsigned char buffer[], int len)
//{
//	unsigned char data = 0;
//	int i = 0;
//	for (; i < len; i++)
//		data ^= buffer[i];
//
//	return data;
//}
//
//static time_t GetTick(char *strtime)
//{
//	struct tm stm;
//	int iY, iM, iD, iH, iMin, iS;
//	memset(&stm, 0, sizeof(stm));
//	sscanf(strtime, "%04d-%02d-%02d %02d:%02d:%02d", &iY, &iM, &iD, &iH, &iMin, &iS);
//	stm.tm_year = iY - 1900;
//	stm.tm_mon = iM - 1;
//	stm.tm_mday = iD;
//	stm.tm_hour = iH;
//	stm.tm_min = iMin;
//	stm.tm_sec = iS;
//	return mktime(&stm);
//}
//
//char* addfriend_jsonstring(char *fromid, char *toid, char *seqno)
//{
//	char  * rslt = NULL;
//	cJSON * item = NULL;
//	cJSON * jarray = NULL;
//	char    szvalue[128];
//	usertable_t userinfo = usertable_new();
//	cJSON * root = cJSON_CreateObject();
//	cJSON_AddStringToObject(root, "type", "invitefriend");
//	//db_query_userinfo(client()->sqlobj, atoi(fromid), userinfo);
//	snprintf(szvalue, sizeof(szvalue), "%s", userinfo->name ? userinfo->name : "NULL");
//	free(userinfo->name);
//	cJSON * fromjson = cJSON_CreateObject();
//	cJSON_AddStringToObject(fromjson, "name", szvalue);
//	snprintf(szvalue, sizeof(szvalue), "%d", userinfo->id);
//	cJSON_AddStringToObject(fromjson, "id", szvalue);
//	cJSON_AddItemToObject(root, "from", fromjson);
//	cJSON_AddStringToObject(root, "to", toid);
//	cJSON_AddStringToObject(root, "seqno", seqno);
//	rslt = cJSON_Print(root);
//	usertable_free(userinfo);
//	free(root);	  //避免内存泄漏
//	return rslt;
//}
//
//char* acceptfriend_jsonstring(char *fromid, char*toid, char *acceptmsg, char *seqno) //输入参数为原始串
//{
//	char  * rslt = NULL;
//	cJSON * item = NULL;
//	cJSON * jarray = NULL;
//	char    szvalue[128];
//	char *nickname = NULL;
//
//	usertable_t userinfo = usertable_new();
//	cJSON * root = cJSON_CreateObject();
//	cJSON_AddStringToObject(root, "type", "newfriend"); //平台发送认可通知
//	//db_query_userinfo(client()->sqlobj, atoi(fromid), userinfo);
//	snprintf(szvalue, sizeof(szvalue), "%s", userinfo->name ? userinfo->name : "NULL");
//	free(userinfo->name);
//	cJSON * fromjson = cJSON_CreateObject();
//	cJSON_AddStringToObject(fromjson, "name", szvalue);
//	snprintf(szvalue, sizeof(szvalue), "%d", userinfo->id);
//	cJSON_AddStringToObject(fromjson, "id", szvalue);
//	snprintf(szvalue, sizeof(szvalue), "%s", userinfo->phoneno ? userinfo->phoneno : "NULL");
//	free(userinfo->phoneno);
//	cJSON_AddStringToObject(fromjson, "phoneno", szvalue);
//	snprintf(szvalue, sizeof(szvalue), "%d", userinfo->bid);
//	cJSON_AddStringToObject(fromjson, "bdid", szvalue);
//	cJSON_AddItemToObject(root, "userinfo", fromjson);
//	cJSON_AddStringToObject(root, "to", toid);
//	cJSON_AddStringToObject(root, "seqno", seqno);
//	cJSON_AddStringToObject(root, "accept", acceptmsg);
//	rslt = cJSON_Print(root);
//	usertable_free(userinfo);
//	return rslt;
//}
////分析缓存中的数据内容，并处理
void  router_exp_cb(int msgid, void* msg, int itype, int pid, int len, void* param)
{
//	sess_t sess;
//	p2pclient_t  p2p = 0;
//	int  msgcode = 0;
//
//	char ckey[48] = { 0 };
//	unsigned char* frame = (unsigned char*)msg;
//	router_exp_t exp = (router_exp_t)param;
//	sess = (sess_t)_BASE(exp).param;
//	p2p = (p2pclient_t)sess;
//
//	if (exp == NULL || sess == NULL) return;
//
//	switch (msgid)
//	{
//	case cunpack_active:
//	{
//		char *ip, *port;
//		cJSON  *redisroot;
//		BOOL   bdactine = FALSE;
//		sprintf(ckey, "%d_active", pid);
//		char *contentstr = NULL;
//		char rediscontent[528] = { 0 };
//		contentstr = process_get_str_str(g_rediscontext, ckey);
//		if (contentstr  == NULL) //如果没有数据
//		{
//			process_push_str(g_rediscontext, ckey, frame); //直接插入
//			break;
//		}
//		else
//		{
//			strcpy(rediscontent, contentstr);
//			free(contentstr);
//		}
//		
//		//解析json串
//		cJSON  *jsonroot = cJSON_Parse(frame); //北斗时间 or 终端时间
//		if (jsonroot == NULL) return;
//
//		cJSON *json = cJSON_GetObjectItem(jsonroot, "bdactivetime");
//		if (json == NULL)
//		{
//			json = cJSON_GetObjectItem(jsonroot, "inetactivetime"); //互联网在线信息
//			if (json == NULL)
//				return;
//			bdactine = FALSE;
//			cJSON *ipjson = cJSON_GetObjectItem(jsonroot, "ip");
//			if (ipjson != NULL)
//				ip = ipjson->valuestring;
//			ipjson = cJSON_GetObjectItem(jsonroot, "port");
//			if (ipjson != NULL)
//				port = ipjson->valuestring;
//		}
//		else{
//			bdactine = TRUE;
//
//		}
//		char *timespan = json->valuestring;
//		if (timespan != "")
//		{
//			redisroot = cJSON_Parse(rediscontent);
//			if (bdactine == TRUE) //北斗在线信息
//			{
//				json = cJSON_GetObjectItem(redisroot, "bdactivetime"); //缓存中没有bdactive
//				if (json == NULL)
//				{
//					//追加
//					cJSON * jsonobject = cJSON_CreateObject();
//					cJSON_AddStringToObject(redisroot, "bdactivetime", timespan);
//				}
//				else
//				{	
//					cJSON  *jstring = cJSON_CreateString(timespan);
//					cJSON_ReplaceItemInObject(redisroot, "bdactivetime", jstring);
//				}
//				process_push_str(g_rediscontext, ckey, cJSON_Print(redisroot)); //直接插入
//			}
//			else //互联网在线信息
//			{
//				json = cJSON_GetObjectItem(redisroot, "inetactivetime");
//				if (json == NULL)
//				{
//					//追加
//					cJSON * jsonobject = cJSON_CreateObject();
//					cJSON_AddStringToObject(redisroot, "inetactivetime", timespan);
//					cJSON_AddStringToObject(redisroot, "ip", ip);
//					cJSON_AddStringToObject(redisroot, "port", port);
//				}else //修改
//				{
//					cJSON  *jstring = cJSON_CreateString(timespan);
//					cJSON_ReplaceItemInObject(redisroot, "inetactivetime", jstring);
//					jstring = cJSON_CreateString(ip);
//					cJSON_ReplaceItemInObject(redisroot, "ip", jstring);
//					jstring = cJSON_CreateString(port);
//					cJSON_ReplaceItemInObject(redisroot, "port", jstring);
//
//				}
//				process_push_str(g_rediscontext, ckey, cJSON_Print(redisroot)); //直接插入
//			}
//		}
//		if (itype == 0)
//			process_bd_smcache(pid); 
//		else if (itype == 1)
//			process_inet_smcache(pid);
//	}
//	break;
//	case cunpack_inetmsg:
//	{
//		int iactive = ispresence(pid, 1);
//		sprintf(ckey, "%d_inetmsg", pid);
//		process_push_lst(g_rediscontext, ckey, frame);
//		if (iactive == 1 ) //15s 内是活跃的，认为是在线的
//		{
//			process_inet_smcache(pid); //将缓存中的内容发送出去
//		}
//	}
//	break;
//	case cunpack_bdmail:
//	{
//		sprintf(ckey, "%d_bdmail", pid);
//		process_push_lst(g_rediscontext, ckey, frame);
//	}
//	break;
//	case cunpack_sos:
//	{
//		sprintf(ckey, "%d_sos", pid);
//		process_push_lst(g_rediscontext, ckey, frame);
//	}
//	break;
//	case cunpack_pos:
//	{
//		sprintf(ckey, "%d_pos", pid);
//		process_push_lst(g_rediscontext, ckey, frame);
//	}
//	break;
//	case cunpack_bdposrpt:
//	{
//		sprintf(ckey, "%d_bdposrpt", pid);
//		process_push_lst(g_rediscontext, ckey, frame);
//	}
//	break;
//	case cunpack_delete: //删除
//	{
//		if (itype == 1 || itype == 2)
//		{
//			sprintf(ckey, "%d_inetmsg", pid);
//			//删除缓存
//			cJSON *jsonroot = cJSON_Parse(frame);
//			if (jsonroot == NULL)
//			{
//				cJSON_Delete(jsonroot);
//				break;
//			}
//			cJSON *json = cJSON_GetObjectItem(jsonroot, "type");
//			if (json == NULL)
//			{
//				cJSON_Delete(jsonroot);
//				break;
//			}
//			json = cJSON_GetObjectItem(jsonroot, "seqno"); //
//			if (json == NULL)
//			{
//				cJSON_Delete(jsonroot);
//				return ;
//			}
//			char *seqno = json->valuestring;
//			int inum = atoi(seqno);
//			cJSON *jsonfrom = cJSON_GetObjectItem(jsonroot, "userinfo"); 
//			if (jsonfrom == NULL)
//			{
//				jsonfrom = cJSON_GetObjectItem(jsonroot, "from");
//				if (jsonfrom == NULL)
//				{
//					cJSON_Delete(jsonroot);
//					return ;
//				}
//			}
//			json = cJSON_GetObjectItem(jsonfrom, "id");
//			if (json == NULL)
//			{
//				cJSON_Delete(jsonroot);
//				return ;
//			}
//			int ifromid = atoi(json->valuestring);
//			json = cJSON_GetObjectItem(jsonroot, "to");
//			if (json == NULL)
//			{
//				cJSON_Delete(jsonroot);
//				return ;
//			}
//			int itoid = atoi(json->valuestring);
//			process_del_item_friend(g_rediscontext, ckey, ifromid, itoid, inum);
//			break;
//
//		}
//		else  if (itype == 3 || itype == 4 || itype == 9 ||itype == 10)
//		{
//			sprintf(ckey, "%d_inetmsg", pid);
//		}
//		else if (itype == 5)
//		{
//			sprintf(ckey, "%d_bdmail", pid);
//		}
//		else if (itype == 6)
//		{
//			sprintf(ckey, "%d_pos", pid);
//		}
//		else if (itype == 7)
//		{
//			sprintf(ckey, "%d_bdposrpt", pid);
//		}
//		else if (itype == 8)
//		{
//			sprintf(ckey, "%d_pos", pid);
//		}
//		cJSON *jsonroot = cJSON_Parse(frame);
//
//		if (jsonroot == NULL) return ;
//		cJSON *json = cJSON_GetObjectItem(jsonroot, "seqno"); //
//		if (json == NULL)
//		{
//			cJSON_Delete(jsonroot);
//			log_write(client()->log, LOG_NOTICE, "del %s failed\r\n", ckey);
//			return;
//		}
//		int num = atoi(json->valuestring);
//		int ifromid = 0;
//		json = cJSON_GetObjectItem(jsonroot, "from");
//		if (json != NULL)
//		{
//			ifromid = atoi(json->valuestring);
//		}
//		else
//		{
//			cJSON_Delete(jsonroot);
//			log_write(client()->log, LOG_NOTICE, "del %s failed\r\n", ckey);
//			return;
//		}
//		cJSON_Delete(jsonroot);
// 		process_del_item(g_rediscontext, ckey, ifromid, num);
//		DeleteMsg(pid, num);
//	}
//	default:
//		break;
//
//	}
//
}
//
//void DeleteMsg(int ipid, int iseqno)
//{
//	for (int i = 0; i < g_inetmsgcount; i++)
//	{
//		inetmsg_st msg = g_InetMsg[i];
//		if (msg.pid == ipid && msg.seqno == iseqno)
//		{
//			ReSortMsg(i);
//			break;
//		}
//	}
//}
//
//time_t GetLatSendtime(int ipid)
//{
//	for (int i = 0; i < g_peresencecount; i++)
//	{
//		inetpresence_st msg = g_inetperesence[i];
//		if (msg.pid == ipid)
//		{
//			return msg.lastsendtime;
//			break;
//		}
//	}
//	return 0;
//}
//
//void   InsertPresence(int pid,time_t sendtime)
//{
//	BOOL bFind = FALSE;
//	for (int i = 0; i < g_peresencecount; i++)
//	{
//		if (g_inetperesence[i].pid == pid)
//		{
//			bFind = TRUE;
//			g_inetperesence[i].lastsendtime = sendtime;
//			break;
//		}
//		if (g_inetperesence[i].pid == 0)  break;
//	}
//	if (bFind == FALSE)
//	{
//		g_inetperesence[g_peresencecount].pid = pid;
//		g_inetperesence[g_peresencecount].lastsendtime = sendtime;
//		g_peresencecount++;
//	}
//}
//void  ReSortMsg(int idel) //删除一个之后重新排序
//{
//	if (idel == g_inetmsgcount - 1) return;
//	for (int i = idel; i < g_inetmsgcount-1; i++)
//	{
//		g_InetMsg[i] = g_InetMsg[i + 1];
//	}
//	g_inetmsgcount--;
//	if (g_inetmsgcount < 0) g_inetmsgcount = 0;
//}
////redis->本地缓存
//void  HandleErrorInetMsg(int pid,int iseqno)
//{
//	BOOL bfind = FALSE;
//	char ckey[24] = { 0 };
//	for (int i = 0; i < g_inetmsgcount; i++)
//	{
//		inetmsg_st msg = g_InetMsg[i];
//		if (msg.pid == pid && msg.seqno == iseqno)
//		{
//			bfind = TRUE;
//			g_InetMsg[i].inum++;
//			if (g_InetMsg[i].inum > MAX_ERROR_NUM)//三次机会，还是没有收到，则删除
//			{
//				//处理缓存
//				ReSortMsg(i); //
//				sprintf(ckey, "%d_inetmsg", pid);
//				process_del_lst(g_rediscontext, ckey); //
//				
//			}
//			break;
//		}
//
//	}
//	if (bfind == FALSE)
//	{
//		int index = g_inetmsgcount;
//		g_InetMsg[index].pid = pid;
//		g_InetMsg[index].seqno = iseqno;
//		g_InetMsg[index].inum = 1;
//		g_inetmsgcount++;
//	}
//}
////查询"PID_inetmsg"、"PID_sos"、"PID_bdposrpt"缓存列表信息，如果存在未读取的信息则读取并完成数据推送。
//void  process_inet_smcache(int pid) //处理缓存中的数据(每项一次推送5条)
//{
//	//待处理 需要加消息序列号，平台返回后，从缓存中删除此条消息
//	if (((p2pclient_t)client()->p2prouter)->lost == 1)
//	{
//		return; //因为缓存内容会被删除，所以需要router在线
//	}
//	response_pkt_p pkt;
//	cJSON *jsonroot, *json;
//	char message[1024] = { 0 };
//	char ckey[48] = { 0 };
//	int idatalen = 0;
//	int inetmsg_count = 0;
//	int isos_count = 0;
//	int ibdposrpt = 0;
//	int ibdmail_count = 0;
//	char strip[64] = { 0 };
//	int  iport = 0;
//	int  ipushcount = 0;//
//	sprintf(ckey, "%d_active", pid);
//	char *activemessage = process_get_str_str(g_rediscontext, ckey);
//	strcpy(message, activemessage);
//	free(activemessage);
//	jsonroot = cJSON_Parse(message);
//	if (jsonroot == NULL) return;
//	json = cJSON_GetObjectItem(jsonroot, "ip");
//	if (json != NULL)
//	{
//		strcpy(strip, json->valuestring);
//	}
//	json = cJSON_GetObjectItem(jsonroot, "port");
//	if (json != NULL)
//	{
//		iport = atoi(json->valuestring);
//	}
//
//	cJSON_Delete(jsonroot);
//	
//	int imsgdex = 0;
//	sprintf(ckey, "%d_inetmsg", pid);
//	inetmsg_count = process_cout(g_rediscontext, ckey);
//	
//	time_t sendtime = 0;
//	sendtime = time(0);
//	time_t lastsendtime = GetLatSendtime(pid);
//	//if (sendtime - lastsendtime < 3) //小于3秒 不处理<防止频繁发送presence>
//	//{
//	//	return;
//	//}
//	//
//	InsertPresence(pid, sendtime);
//	while (inetmsg_count > 0)
//	{
//		char cfriendbuffer[1024] = { 0 };
//		char *toid = NULL;
//		char *seqno = NULL;
//		char *fromid = NULL;
//		char *acceptmsg = NULL;
//		idatalen = 0;
//		char *inetmsgtemp = process_get_str(g_rediscontext, ckey, imsgdex);
//		if (inetmsgtemp == NULL)
//		{
//			return;
//		}
//		jsonroot = cJSON_Parse(inetmsgtemp);
//		if (jsonroot == NULL)
//		{
//			free(inetmsgtemp);
//			return;
//		}
//
//		json = cJSON_GetObjectItem(jsonroot, "type");
//		if (json == NULL)
//		{
//			cJSON_Delete(jsonroot);
//			free(inetmsgtemp);
//			return;
//		}
//		char *msgtype = json->valuestring;
//		if (strcmp(msgtype, "addfriend") == 0)
//		{
//			json = cJSON_GetObjectItem(jsonroot, "id");
//			if (json == NULL)
//			{
//				cJSON_Delete(jsonroot);
//				free(inetmsgtemp);
//				return;
//			}
//			toid = json->valuestring;
//			
//		}
//		json = cJSON_GetObjectItem(jsonroot, "seqno");
//		if (json == NULL)
//		{
//			cJSON_Delete(jsonroot);
//			free(inetmsgtemp);
//			return;
//		}
//		seqno = json->valuestring;
//		HandleErrorInetMsg(pid, atoi(seqno));
//		if (strcmp(msgtype, "addfriend") == 0)
//		{
//			json = cJSON_GetObjectItem(jsonroot, "from");
//			if (json == NULL)
//			{
//				cJSON_Delete(jsonroot);
//				free(inetmsgtemp);
//				return;
//			}
//			fromid = json->valuestring;
//			strcpy(cfriendbuffer, addfriend_jsonstring(fromid, toid, seqno)); //重新赋值
//			cJSON_Delete(jsonroot);
//			idatalen += sprintf_s(g_msgcache + idatalen, sizeof(g_msgcache) - idatalen, "$message,%s,%s,{\"ip\":\"%s\",\"port\":\"%d\",\"response\":%s}", "smcache", "router", strip, iport, cfriendbuffer);
//		}
//		else if (strcmp(msgtype, "acceptfriend") == 0)
//		{
//			cJSON *fromjson = cJSON_GetObjectItem(jsonroot, "from");
//			if (fromjson == NULL)
//			{
//				cJSON_Delete(jsonroot);
//				free(inetmsgtemp);
//				return;
//			}
//
//			json = cJSON_GetObjectItem(fromjson, "id");
//			if (json == NULL)
//			{
//				cJSON_Delete(jsonroot);
//				free(inetmsgtemp);
//				return;
//			}
//			fromid = json->valuestring;
//			json = cJSON_GetObjectItem(jsonroot, "to");
//			if (json == NULL)
//			{
//				cJSON_Delete(jsonroot);
//                free(inetmsgtemp);
//                return;
//			}
//			toid = json->valuestring;
//
//			json = cJSON_GetObjectItem(jsonroot, "seqno");
//			if (json == NULL)
//			{
//				cJSON_Delete(jsonroot);
//				free(inetmsgtemp);
//				return;
//			}
//			seqno = json->valuestring;
//			json = cJSON_GetObjectItem(jsonroot, "accept");
//			if (json == NULL)
//			{
//				cJSON_Delete(jsonroot);
//				free(inetmsgtemp);
//				return;
//			}
//			acceptmsg = json->valuestring;
//			strcpy(cfriendbuffer, acceptfriend_jsonstring(fromid, toid, acceptmsg, seqno));
//			cJSON_Delete(jsonroot);
//			idatalen += sprintf_s(g_msgcache + idatalen, sizeof(g_msgcache) - idatalen, "$message,%s,%s,{\"ip\":\"%s\",\"port\":\"%d\",\"response\":%s}", "smcache", "router", strip, iport, cfriendbuffer);
//		}
//		else
//		{
//			idatalen += sprintf_s(g_msgcache + idatalen, sizeof(g_msgcache) - idatalen, "$message,%s,%s,{\"ip\":\"%s\",\"port\":\"%d\",\"response\":%s}", "smcache", "router", strip, iport, inetmsgtemp);
//		}
//		idatalen += sprintf_s(g_msgcache + idatalen, sizeof(g_msgcache) - idatalen, "*%02X\r\n", checksum(g_msgcache, idatalen));
//		free(inetmsgtemp);
//		inetmsgtemp = NULL;
//		pkt = response_pkt_new(client()->pktpool, idatalen);
//		memcpy(pkt->data, g_msgcache, idatalen);
//		pkt->len = idatalen;
//		p2pclient_write((p2pclient_t)client()->p2prouter, pkt);//打包发送给router
//		printf("send to router %s\r\n", g_msgcache);
//		log_write(client()->log, LOG_NOTICE, "send inetmsg to router :%s .\n", g_msgcache);
//		inetmsg_count--;
//		imsgdex++;
//		ipushcount++;
//		if (ipushcount >= 10)
//			break;
//	}
//	ipushcount = 0;
//	imsgdex = 0;
//	sprintf(ckey, "%d_sos", pid);
//	isos_count = process_cout(g_rediscontext, ckey);
//	while (isos_count > 0)
//	{
//		idatalen = 0;
//		char *mailcache = process_get_str(g_rediscontext, ckey, imsgdex);
//		if (mailcache != NULL)
//		{
//			idatalen += sprintf_s(g_msgcache + idatalen, sizeof(g_msgcache) - idatalen, "$message,%s,%s,{\"response\":%s}", "smcache", "router", mailcache);
//			free(mailcache);
//			idatalen += sprintf_s(g_msgcache + idatalen, sizeof(g_msgcache) - idatalen, "*%02X\r\n", checksum(g_msgcache, idatalen));
//			pkt = response_pkt_new(client()->pktpool, idatalen);
//			memcpy(pkt->data, g_msgcache, idatalen);
//			pkt->len = idatalen;
//			p2pclient_write((p2pclient_t)client()->p2prouter, pkt);//打包发送给router
//			process_del_lst(g_rediscontext, ckey); //直接删除 不需要答复
//		}
//		isos_count--;
//		imsgdex++;
//		ipushcount++;
//		if (ipushcount >= 5)
//			break;
//	}
//}
//
//void  process_bd_smcache(int pid) //处理缓存中发往北斗数据(每项一次推送3条)
//{
//	if (((p2pclient_t)client()->p2prouter)->lost == 1)
//	{
//		return;
//	}
//	response_pkt_p pkt;
//	char message[1024] = { 0 };
//	char ckey[48] = { 0 };
//	int idatalen = 0;
//	int ibdmail_count = 0;
//	char strip[64] = { 0 };
//	int  ipushcount = 0;//
//	int imsgdex = 0;
//	sprintf(ckey, "%d_bdmail", pid);
//	ibdmail_count = process_cout(g_rediscontext, ckey);
//	while (ibdmail_count > 0)
//	{
//		idatalen = 0;
//		char *mailcache = process_get_str(g_rediscontext, ckey, imsgdex);
//		if (mailcache != NULL)
//		{
//			idatalen += sprintf_s(g_msgcache + idatalen, sizeof(g_msgcache) - idatalen, "$message,%s,%s,{\"response\":%s}", "smcache", "router", mailcache);
//			free(mailcache);
//			idatalen += sprintf_s(g_msgcache + idatalen, sizeof(g_msgcache) - idatalen, "*%02X\r\n", checksum(g_msgcache, idatalen));
//			pkt = response_pkt_new(client()->pktpool, idatalen);
//			memcpy(pkt->data, g_msgcache, idatalen);
//			pkt->len = idatalen;
//			
//			p2pclient_write((p2pclient_t)client()->p2prouter, pkt);//打包发送给router
//			log_write(client()->log, LOG_NOTICE, "send bdmail to router :%s .\n", g_msgcache);
//			process_del_lst(g_rediscontext, ckey); //直接删除 不需要答复
//		}
//		else
//		{
//			imsgdex++;
//		}
//		ibdmail_count--;
//		
//		ipushcount++;
//		if (ipushcount >= 3) //一次最多推送三条
//			break;
//	}
//}
////查看活动时间是否超过5秒
//int  ispresence(int pid, int inet)
//{
//	cJSON *jsonroot, *json;
//	char *activetime;
//	char cmessage[1024 * 64] = { 0 };
//	char ckey[48] = { 0 };
//	char buffertime[64] = { 0 };
//	sprintf(ckey, "%d_active", pid);
//	char *activemessage = process_get_str_str(g_rediscontext, ckey);
//	if (activemessage == NULL) return 0;
//	strcpy(cmessage, activemessage);
//	free(activemessage);
//	if (cmessage == "") return 0;
//	//解析json串，获得活动时间
//	jsonroot = cJSON_Parse(cmessage);
//	if (inet == 1)
//	{
//		json = cJSON_GetObjectItem(jsonroot, "inetactivetime");
//		if (json == NULL)
//		{
//			cJSON_Delete(jsonroot);
//			return 0;
//		}
//		activetime = json->valuestring;
//	}
//	else
//	{
//		json = cJSON_GetObjectItem(jsonroot, "bdactivetime");
//		if (json == NULL)
//		{
//			cJSON_Delete(jsonroot);
//			return 0;
//		}
//		activetime = json->valuestring;
//	}
//	if (activetime == NULL)
//		return 0;
//	time_t  oldactivetime = GetTick(activetime);
//	time_t  now = time(0);
//	time_t dis = now - oldactivetime;
//	if (dis > 17)  return 0;
//
//	return 1;
//}