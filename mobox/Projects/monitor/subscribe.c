#include "subscribe.h"

extern xht g_xsubbytaskid;

static void xhash_walk1(xht h, const char *key, void *val, void *arg)
{
	printf("key:%s val:%p\n", key, val);
}

// 向全局队列中加入taskid项
char * add_taskid_to_queue(taskid)
{
	char *task;
	jqueue_t q = client()->taskid;
	if (q == NULL)
		q = client()->taskid = jqueue_new();

	// 遍历，避免有重复taskid存在
	int i = 0;
	int size = jqueue_size(q);
	for (i = 0; i < size; i++)
	{
		task = (char*)jqueue_pull(q);
		if (strcmp(task, taskid) == 0)
		{
			jqueue_push(q, task, 0);
			free(taskid);
			return task;
		}
		jqueue_push(q, task, 0);
	}

	task = taskid;
	jqueue_push(client()->taskid, task, 0);
	return task;
}

// 从全局队列中移除taskid
void del_taskid_from_queue(taskid)
{
	char *task;
	jqueue_t q = client()->taskid;
	if (q == NULL)
		q = client()->taskid = jqueue_new();

	// 遍历，避免有重复taskid存在
	int i = 0;
	int size = jqueue_size(q);
	for (i = 0; i < size; i++)
	{
		task = (char*)jqueue_pull(q);
		if (strcmp(task, taskid) == 0)
		{
			jqueue_push(q, task, 0);
			free(taskid);
			return task;
		}
		jqueue_push(q, task, 0);
	}

	task = taskid;
	jqueue_push(client()->taskid, task, 0);
	return task;
}

// 将sess放入tasikd对应的连接队列中
int add_sess_to_subtable(xht ht, char * taskid, sess_t sess)
{
	jqueue_t q = (jqueue_t)xhash_get(ht, taskid);
	if (q == NULL)
	{
		q = jqueue_new();
		if (q == NULL)
		{
			return -1;
		}
		xhash_put(ht, taskid, q);		// 此处taskid为堆上的
	}

	q = (jqueue_t)xhash_get(ht, taskid);

	//xhash_walk(ht, xhash_walk1, NULL);

	// 遍历队列，确认其中没有相同的sess存在，方可插入，避免重复
	sess_t s;
	int i = 0;
	int size = jqueue_size(q);

	for (i = 0; i < size; i++)
	{
		s = (sess_t)jqueue_pull(q);
		if (s == sess)
			continue;
		jqueue_push(q, s, 0);
	}


	jqueue_push(q, sess, 0);

	return 0;
}

// 将taskid从client->taskid队列中移除
void clear_from_taskid_queue(char * taskid)
{
	char *task;
	jqueue_t q = client()->taskid;
	if (q == NULL)
		return;
	int size = jqueue_size(q);

	for (int i = 0; i < size; i++)
	{
		task = (char*)jqueue_pull(q);
		if (strcmp(taskid, task) == 0)
		{
			free(task);
			break;
		}
		jqueue_push(q, task, 0);
	}
}

// 将sess从tasikd对应的连接队列中移除
int del_sess_from_subtable(xht ht, char * taskid, sess_t sess)
{
	jqueue_t q = (jqueue_t)xhash_get(ht, taskid);
	if (q == NULL)
		return -1;

	// 遍历队列，确保相同sess全部被移除
	sess_t s;
	int i = 0;
	int size = jqueue_size(q);
	for (i = 0; i < size; i++)
	{
		s = (sess_t)jqueue_pull(q);
		if (s == sess) continue;

		jqueue_push(q, s, 0);
	}

	// 移除sess之后，检验taskid下还有没有sess，如果没有，则表明没有订阅者，该taskid可以被注销释放
	q = (jqueue_t)xhash_get(ht, taskid);
	if (q == NULL)
	{
		return -1;
	}

	size = jqueue_size(q);
	if (jqueue_size(q) == 0)
	{
		xhash_zap(ht, taskid);
		clear_from_taskid_queue(taskid);		// 从client->taskid队列中移除
		jqueue_free(q);
	}
	else
	{
		for (i = 0; i < size; i++)
		{
			s = (sess_t)jqueue_pull(q);
			//log_write(client()->log, LOG_NOTICE, "task[%s] is subed by sess:%s.", i, sess->sname);
			jqueue_push(q, s, 0);
		}
		
	}

	return 0;
}

// 向sess维护的subs队列中，插入订阅目标taskid
int add_subs_to_sess(sess_t sess, char *taskid)
{
	jqueue_t q = sess->subs;
	if (q == NULL)
		q = sess->subs = jqueue_new();

	// 遍历队列，确认其中没有相同的taskid存在，方可插入，避免重复
	int i = 0;
	char *task;
	int size = jqueue_size(q);
	for (i = 0; i < size; i++)
	{
		task = (char*)jqueue_pull(q);
		if (strcmp(task, taskid) == 0)
		{
			jqueue_push(q, task, 0);
			return 1;
		}
			
		jqueue_push(q, task, 0);
	}

	jqueue_push(q, taskid, 0);		// 此处的taskid是堆上的

	return 0;
}

// 将该taskid从sess对应的任务队列sess->subs中移除
int del_subs_from_sess(sess_t sess, char *taskid)
{
	sess_t s;
	jqueue_t q = sess->subs;
	if (q == NULL)
		return -1;

	// 遍历队列，确保所有与taskid相同项全部被移除
	int i = 0;
	char *task;
	int size = jqueue_size(q);
	for (i = 0; i < size; i++)
	{
		task = (char*)jqueue_pull(q);
		if (strcmp(task, taskid) == 0)
			continue;
		jqueue_push(q, taskid, 0);
	}

	return 0;
}





// 将sess从其订阅的每个taskid对应的队列中移除
int clear_sess_from_eachtaskid(sess_t sess)
{
	if (sess->subs == NULL || jqueue_size(sess->subs) == 0)
		return -1;

	int i = 0;
	char * taskid;
	xht ht = g_xsubbytaskid;
	int size = jqueue_size(sess->subs);	
	jqueue_t q;

	for (i = 0; i < size; i++)
	{
		taskid = (char *)jqueue_pull(sess->subs);	// 找出订阅的某个taskid
		del_sess_from_subtable(ht, taskid, sess);	// 将sess从该taskid对应的sess队列中删除
	}

	return 0;
}