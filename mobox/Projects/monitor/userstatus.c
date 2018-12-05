#include "userstatus.h"

user_status_t user_status_clone(user_status_t us)
{
	user_status_t r = NULL;

	if (us == NULL) return NULL;

	r = malloc(sizeof(user_status_st));

	memcpy(r, us, sizeof(user_status_st));

	return r;
}
	
users_act_t users_act_new(int prime)
{
	users_act_t r = NULL;

	while ((r = calloc(1, sizeof(users_act_st))) == NULL) Sleep(1);

	r->_q = jqueue_new();
	r->_t = xhash_new(prime);

	return r;
}

void users_act_free(users_act_t us)
{
	if (us == NULL) return;
	
	if (us->_q){
		int count = jqueue_size(us->_q);

		for (int i = 0; i < count; i++) free(jqueue_pull(us->_q));			
		
		jqueue_free(us->_q);
	}

	if (us->_q) xhash_free(us->_t);

	free(us);	
}

int   users_act_set(users_act_t us, user_status_t stat)
{
	if (us == NULL || stat == NULL) return 0;

	user_status_t s = xhash_get(us->_t, stat->key);

	if (s == NULL){
		s = user_status_clone(stat);
		xhash_put(us->_t, stat->key, s);
		jqueue_push(us->_q, s, 0);
	}
	
	s->status = stat->status;	

	return 1;
}

user_status_t  users_act_get(users_act_t us, char* key)
{
	if (us == NULL || key == NULL) return NULL;

	return xhash_get(us->_t, key);
}

void  users_act_del(users_act_t us, char* key)
{
	if (us == NULL || key == NULL) return;

	user_status_t s = xhash_get(us->_t, key);
	if (s == NULL) return;

	xhash_zap(us->_t, key);

	int cnt = jqueue_size(us->_q);

	for (int i = 0; i < cnt; i++){
		s = jqueue_pull(us->_q);

		if (s == NULL) continue;

		if (strcmp(s->key, key) == 0){free(s);break;}

		jqueue_push(us->_q, s, 0);
	}
}

void xhash_walk1(xht h, const char *key, void *val, void *arg)
{
	printf("key:%s val:%s\n", key, val);
}

void  users_act_save(users_act_t us, char* key, int sec, int type)
{
	if (us == NULL || key == NULL) return;

	user_status_t s = xhash_get(us->_t, key);	

	if (s == NULL){
		s = calloc(1, sizeof(user_status_st));

		strcpy_s(s->key, sizeof(s->key), key);

		xhash_put(us->_t, s->key, s);
		jqueue_push(us->_q, s, 0);
	}
	
	s->type   = type;
	s->life   = sec;
	s->status = _status_online;	
}

jqueue_t users_act_check(users_act_t us, int sec)
{
	static time_t last = 0;
	jqueue_t deads = NULL;

	if (us == NULL) return NULL;	
	
	time_t now = time(0);

	if (last == 0) last = now;

	if (now - last < sec) return NULL;	

	int cnt = jqueue_size(us->_q);

	for (int i = 0; i < cnt; i++){
		user_status_t s = jqueue_pull(us->_q);

		s->life -= sec;

		if (s->life < 0){

			xhash_zap(us->_t, s->key);

			if(deads == NULL) deads = jqueue_new();

			jqueue_push(deads, s, 0); 			 
		}
		else{
			jqueue_push(us->_q, s, 0);			
		}
	}
	last = now;
	return deads;
}