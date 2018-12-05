#ifndef _MEMORY_POOL_H
#define _MEMORY_POOL_H

#include "malloc.h"
#include "memory.h"
#include "stdlib.h"
#include "math.h"
#include "pthread.h"

#define  _MEMPOOL_NFREELISTS 16

#define _FREELIST_INDEX(_bytes, _align_shift) ((_bytes - (size_t)1)>>(int)_align_shift)

typedef union _obj {
	union _obj   *free_list_link;
	char         *pdata; // The client sees this.
}obj, *obj_t;

typedef struct mempool_st
{
	size_t          align;
	size_t          align_shift;
	size_t          max_bytes;
	int             bmt;
	pthread_mutex_t lock;
	
    union _obj *volatile _free_list[_MEMPOOL_NFREELISTS]; 
}*mempool_t;


mempool_t mempool_new(size_t ishift, int bmt);

void      mempool_free(mempool_t mp);

/* __n must be > 0   */
void *allocate(mempool_t mp, size_t __n);
/* __p may not be 0 */
void deallocate(mempool_t mp, void *__p);

void shrink(mempool_t mp);

#endif
