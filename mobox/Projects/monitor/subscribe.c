#include "subscribe.h"

extern xht g_xsubbytaskid;

static void xhash_walk1(xht h, const char *key, void *val, void *arg)
{
	printf("key:%s val:%p\n", key, val);
}

// ��ȫ�ֶ����м���taskid��
char * add_taskid_to_queue(taskid)
{
	char *task;
	jqueue_t q = client()->taskid;
	if (q == NULL)
		q = client()->taskid = jqueue_new();

	// �������������ظ�taskid����
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

// ��ȫ�ֶ������Ƴ�taskid
void del_taskid_from_queue(taskid)
{
	char *task;
	jqueue_t q = client()->taskid;
	if (q == NULL)
		q = client()->taskid = jqueue_new();

	// �������������ظ�taskid����
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

// ��sess����tasikd��Ӧ�����Ӷ�����
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
		xhash_put(ht, taskid, q);		// �˴�taskidΪ���ϵ�
	}

	q = (jqueue_t)xhash_get(ht, taskid);

	//xhash_walk(ht, xhash_walk1, NULL);

	// �������У�ȷ������û����ͬ��sess���ڣ����ɲ��룬�����ظ�
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

// ��taskid��client->taskid�������Ƴ�
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

// ��sess��tasikd��Ӧ�����Ӷ������Ƴ�
int del_sess_from_subtable(xht ht, char * taskid, sess_t sess)
{
	jqueue_t q = (jqueue_t)xhash_get(ht, taskid);
	if (q == NULL)
		return -1;

	// �������У�ȷ����ͬsessȫ�����Ƴ�
	sess_t s;
	int i = 0;
	int size = jqueue_size(q);
	for (i = 0; i < size; i++)
	{
		s = (sess_t)jqueue_pull(q);
		if (s == sess) continue;

		jqueue_push(q, s, 0);
	}

	// �Ƴ�sess֮�󣬼���taskid�»���û��sess�����û�У������û�ж����ߣ���taskid���Ա�ע���ͷ�
	q = (jqueue_t)xhash_get(ht, taskid);
	if (q == NULL)
	{
		return -1;
	}

	size = jqueue_size(q);
	if (jqueue_size(q) == 0)
	{
		xhash_zap(ht, taskid);
		clear_from_taskid_queue(taskid);		// ��client->taskid�������Ƴ�
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

// ��sessά����subs�����У����붩��Ŀ��taskid
int add_subs_to_sess(sess_t sess, char *taskid)
{
	jqueue_t q = sess->subs;
	if (q == NULL)
		q = sess->subs = jqueue_new();

	// �������У�ȷ������û����ͬ��taskid���ڣ����ɲ��룬�����ظ�
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

	jqueue_push(q, taskid, 0);		// �˴���taskid�Ƕ��ϵ�

	return 0;
}

// ����taskid��sess��Ӧ���������sess->subs���Ƴ�
int del_subs_from_sess(sess_t sess, char *taskid)
{
	sess_t s;
	jqueue_t q = sess->subs;
	if (q == NULL)
		return -1;

	// �������У�ȷ��������taskid��ͬ��ȫ�����Ƴ�
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





// ��sess���䶩�ĵ�ÿ��taskid��Ӧ�Ķ������Ƴ�
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
		taskid = (char *)jqueue_pull(sess->subs);	// �ҳ����ĵ�ĳ��taskid
		del_sess_from_subtable(ht, taskid, sess);	// ��sess�Ӹ�taskid��Ӧ��sess������ɾ��
	}

	return 0;
}