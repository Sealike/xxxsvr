#pragma once
#include "util/xhash.h"
#include "util/util.h"

typedef struct _subject_st{
	xht _ht;
}subject_st, *subject_t;

subject_t subject_new(int cap);
void      subject_free(subject_t subject);

int subject_add(subject_t subject, char* topic, void* sess);
int subject_del(subject_t subject, char* topic, void* sess);
void* subject_get(subject_t subject, char* topic);



