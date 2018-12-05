#ifndef _FORWARD_H_
#define _FORWARD_H_

#include "p2p.h"
#include "json/cJSON.h"
#include "client.h"
#include "p2pclient.h"
#include "tool.h"
#include "userstatus.h"

// warnsvr_callback
void forward_log_dyncode_to_monitor(char *jstr);
//void forward_dyncode_to_app(char * jstr);
//void forward_warn_to_app(char *jstr);
//
//// bdsvr_callback
//void forward_reqcode_to_warnsvr(char* jstr);
//void forward_log_reqcode_to_monitor(char *jstr);
//void forward_taskpau_to_monitor(char *jstr);
//void forward_log_taskpau_to_monitor(char *jstr);
//void forward_taskcon_to_monitor(char *jstr);
//void forward_log_taskcon_to_monitor(char *jstr);
//void forward_taskdone_to_monitor(char *jstr);
//void forward_log_taskdone_to_monitor(char *jstr);
//
//
//void forward_status_to_monitor(char* jstr);
//void forward_log_to_monitor(char *jstr);
//void forward_pos_to_warnsvr(char *jstr);
//void forward_pos_to_monitor(char *jstr);
//
//// bdsvr and warnsvr callback
//void forward_warn_to_monitor(char * jstr);
//
//// monitor callback
//void forward_taskpau_ack_to_warnsvr(char *jstr);
//void forward_taskpau_ack_to_app(char *jstr);
//void forward_taskcon_ack_to_warnsvr(char *jstr);
//void forward_taskcon_ack_to_app(char *jstr);
//void forward_taskdone_ack_to_warnsvr(char *jstr);
//void forward_taskdone_ack_to_app(char *jstr);

void forward_to_app(char *head, char* jstr);
void forward_to_warnsvr(char *head, char* jstr);
void forward_to_monitor(char *head, char *jstr);	// 将数据直接透传给monitor
void forward_log_x_to_monitor(char *head, char* jstr);// 生成相应的日志，转发给monitor，并且存库

#endif