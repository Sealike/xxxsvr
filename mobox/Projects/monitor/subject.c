#include "subject.h"

subject_t subject_new(int cap)
{
	subject_t rt = NULL;

	while ((rt = (subject_t)calloc(1, sizeof(subject_st))) == NULL) Sleep(1);
	rt->_ht = xhash_new(cap <0 ? 1024 : cap);

	return rt;

}

void      subject_free(subject_t subject)
{
	if (subject == NULL) return;

	if (subject->_ht) {
		void* p = NULL;
		if (xhash_iter_first(subject->_ht)){
			do {
				xhash_iter_get(subject->_ht, NULL, &p);
				if (p) jqueue_free(p);				
			} while (xhash_iter_next(subject->_ht));
		}
		xhash_free(subject->_ht);
	}

	free(subject);
}

int subject_add(subject_t subject,char* topic, void* sess)
{
	if (subject == NULL || topic == NULL || sess == NULL) return 0;

	jqueue_t q = (jqueue_t)xhash_get(subject->_ht, topic);
	if (q == NULL){
		q = (jqueue_t)jqueue_new();		
		xhash_put(subject->_ht, topic, q);
		jqueue_push(q, sess, 0);
	}
	else{
		int i = 0;
		int qsize = jqueue_size(q);

		for (; i < qsize; i++){
			void* p = jqueue_pull(q);
			jqueue_push(q,p,0);
			if (p == sess) break;
		}
		if (i == qsize) jqueue_push(q, sess, 0);
	}

	return 1;
}

int subject_del(subject_t subject, char* topic, void* sess)
{
	if (subject == NULL || topic == NULL || sess == NULL) return 0;

	jqueue_t q = (jqueue_t)xhash_get(subject->_ht, topic);
	if (q != NULL){
		int i = 0;
		int qsize = jqueue_size(q);

		for (; i < qsize; i++){
			void* p = jqueue_pull(q);
			if (p != sess)
				jqueue_push(q, p, 0);			
		}		
	}

	return 1;
}

void* subject_get(subject_t subject, char* topic)
{
	jqueue_t q = (jqueue_t)xhash_get(subject->_ht, topic);

	return q;
}