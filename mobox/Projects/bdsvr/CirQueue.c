#include <stdio.h>
#include "CirQueue.h"

// 函数声明
void InitQueue(Cir_Queue_t Q);
int QueueEmpty(Cir_Queue_t Q);
int QueueFull(Cir_Queue_t Q);
int EnQueue(Cir_Queue_t Q, DataType data);
DataType DeQueue(Cir_Queue_t Q);
DataType QueueFront(Cir_Queue_t Q);
void Error(char * error);

//int main()
//{
//	int i = 200000000;
//	int j = 0;
//	DataType data = 0;
//
//	Cir_Queue_st Q1;
//
//	InitQueue(&Q1);
//
//	while (Q1.nCount < 10)
//	{
//		if (QueueFull(&Q1) == 0)
//			EnQueue(&Q1, data++);
//		printf("EnQueueing to Q1: Q1.rear ==> Q1.data[%d] = %d.  Q1.nCount= %d. Front = %d.\n", Q1.nRear, Q1.data[Q1.nRear], Q1.nCount, Q1.data[Q1.nFront]);
//		while (i-- > 0);
//		i = 200000000;
//	}
//
//	printf("=================\n");
//	while (Q1.nCount > 0)
//	{
//		if (QueueEmpty(&Q1) == 0)
//			printf("DeQueueing from Q1: Q1.front ==> Q1.data[%d] = %d.  Q1.nCount = %d.\n", Q1.nFront, QueueFront(&Q1), Q1.nCount);
//		DeQueue(&Q1);
//		while (i-- > 0);
//		i = 200000000;
//	}
//	printf("=================\n");
//	printf("Q1.nCount= %d\n", Q1.nCount);
//	printf("finish...\n");
//	while (1);
//
//	return 0;
//}

// 置空队
void InitQueue(Cir_Queue_t Q)
{
	Q->nCount = 0;
	Q->nFront = 0;
	Q->nRear = 0;

	for (int i = 0; i < QUEUESIZE; i++)
	{
		Q->data[i] = 0;
	}
}

// 判队空
int QueueEmpty(Cir_Queue_t Q)
{
	return Q->nCount == 0;
}

// 判队满
int QueueFull(Cir_Queue_t Q)
{
	return Q->nCount == QUEUESIZE;
}

// 入队
int EnQueue(Cir_Queue_t Q, DataType data)
{
	if (QueueFull(Q))
	{
		Error("Queue overflow");
		return -1;
	}
	Q->data[Q->nRear] = data;
	Q->nCount++;
	Q->nRear = (Q->nRear + 1) % QUEUESIZE;

	return 0;
}

// 出队
DataType DeQueue(Cir_Queue_t Q)
{
	DataType temp;

	if (QueueEmpty(Q))
	{
		Error("Queue is empty");
		return -1;
	}
	Q->nCount--;
	temp = Q->data[Q->nFront];
	Q->nFront = (Q->nFront + 1) % QUEUESIZE;

	return temp;	
}

// 取出队头
DataType QueueFront(Cir_Queue_t Q)
{
	if (QueueEmpty(Q))
	{
		Error("Queue underflow");
		return -1;
	}
	return Q->data[Q->nFront];
}

void Error(char * error)
{
	printf("error: %s.\n", error);
}





