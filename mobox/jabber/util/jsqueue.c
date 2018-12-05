/* priority jsqueues */

#include "util.h"

jsqueue_t jsqueue_new(void) {
    jsqueue_t q;

    while(NULL == (q = (jsqueue_t) malloc(sizeof(struct _jsqueue_st)))) sleep(1);
    memset(q, 0, sizeof(struct _jsqueue_st));

    q->queue = jqueue_new();
    pthread_mutex_init(&q->lock, NULL);

    return q;
}

void jsqueue_free(jsqueue_t q) {
    assert((int) q);
    jqueue_free(q->queue);
    pthread_mutex_destroy(&q->lock);
    free(q);
}

void jsqueue_push(jsqueue_t q, void *data, int priority) {
    assert((int) q);
    
    pthread_mutex_lock(&q->lock);
    jqueue_push(q->queue, data, priority);
    pthread_mutex_unlock(&q->lock);
}

void *jsqueue_pull(jsqueue_t q) {
    void *data;
    assert((int) q);

    pthread_mutex_lock(&q->lock);
    data = jqueue_pull(q->queue);
    pthread_mutex_unlock(&q->lock);

    return data;
}

int jsqueue_size(jsqueue_t q) {
    int size = 0;

    pthread_mutex_lock(&q->lock);
    size = jqueue_size(q->queue);
    pthread_mutex_unlock(&q->lock);

    return size;
}
