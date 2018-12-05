#ifndef _CIRQUEUE_H_
#define _CIRQUEUE_H_

#ifdef __cplusplus
extern "C"
{
#endif


#define QUEUESIZE 100
typedef int DataType;
typedef struct _Cir_Queue
{
	int nFront;
	int nRear;
	int nCount;
	DataType data[QUEUESIZE];
}Cir_Queue_st, *Cir_Queue_t;

void InitQueue(Cir_Queue_t Q);
int QueueEmpty(Cir_Queue_t Q);
int QueueFull(Cir_Queue_t Q);
int EnQueue(Cir_Queue_t Q, DataType data);
DataType DeQueue(Cir_Queue_t Q);
DataType QueueFront(Cir_Queue_t Q);
void Error(char * error);



#ifdef __cplusplus
}
#endif


#endif