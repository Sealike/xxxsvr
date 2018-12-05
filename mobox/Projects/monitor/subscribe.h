#ifndef __SUBSCRIBE_H__
#define __SUBSCRIBE_H__

#include "client.h"

char * add_taskid_to_queue(taskid);
int add_sess_to_subtable(xht ht, char * taskid, sess_t sess);
int del_sess_from_subtable(xht ht, char * taskid, sess_t sess);
int add_subs_to_sess(sess_t sess, char *taskid);
int del_subs_from_sess(sess_t sess, char *taskid);
int clear_sess_from_eachtaskid(sess_t sess);

#endif