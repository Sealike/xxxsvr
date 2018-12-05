#include "mempool.h"

//_obj * volatile _free_list[_MEMPOOL_NFREELISTS];

static size_t _round_up(mempool_t mp, size_t _bytes)
{
	return (((_bytes) + (size_t)mp->align-1) & ~((size_t)mp->align - 1));
}

static void*  _new(size_t __n) 
{
	void *p = NULL;
	while(NULL == (p = malloc(__n + sizeof(size_t)))) sleep(1);
	*(unsigned int *)p = __n;
	return (unsigned char *)p + sizeof(size_t);
}

static void  _delete(void* __p) 
{
	free((unsigned char *)__p - sizeof(size_t));
}

static void * _pool_allocate(mempool_t mp, size_t __n)
{
	size_t _index_n = 0;
    obj_t pobj = NULL;

	if ( __n <= 0)  return NULL;
	
	__n    = _round_up(mp, __n);
	_index_n = _FREELIST_INDEX(__n, mp->align_shift);
	
	if(mp->bmt) pthread_mutex_lock(&mp->lock);
    if (mp->_free_list[_index_n] == NULL)
    {
		return _new(__n); 
    }
	
	pobj = mp->_free_list[_index_n];
	
	if (pobj->free_list_link == NULL)
	{
		mp->_free_list[_index_n] = NULL;
		return pobj;
	}
	
	mp->_free_list[_index_n] = pobj->free_list_link;
	
	if(mp->bmt) pthread_mutex_unlock(&mp->lock);

    return pobj;
}

static void  _pool_deallocate(mempool_t mp, void *__p, size_t __n)
{
	size_t _index_n = 0;
    obj_t pobj = NULL;

	if ( (__p == NULL) || (__n <= 0))  return;
	
	__n    = _round_up(mp, __n);
	_index_n = _FREELIST_INDEX(__n, mp->align_shift);
	
	if(mp->bmt) pthread_mutex_lock(&mp->lock);
	pobj = mp->_free_list[_index_n];
	
	mp->_free_list[_index_n] = (obj *)__p;
	
    mp->_free_list[_index_n]->free_list_link = pobj;
	
	if(mp->bmt) pthread_mutex_unlock(&mp->lock);

	return;
}

mempool_t mempool_new(size_t ishift, int bmt)
{
	mempool_t mp;
	
    while(NULL == (mp = (mempool_t) malloc(sizeof(struct mempool_st)))) sleep(1);
    memset(mp, 0, sizeof(struct mempool_st));
	
	mp->align_shift = ishift;
	mp->align       = (size_t)pow(2, mp->align_shift);
	mp->max_bytes   = mp->align * 16;
	mp->bmt         = bmt;
	memset((void *)mp->_free_list, 0, _MEMPOOL_NFREELISTS * sizeof(union _obj *)); 
	if(mp->bmt) pthread_mutex_init(&mp->lock, NULL);
	
	return mp;
}

void  mempool_free(mempool_t mp)
{
	shrink(mp);

	if(mp->bmt) pthread_mutex_destroy(&mp->lock);

    free(mp);
}


/* __n must be > 0   */
void *allocate(mempool_t mp, size_t __n) 
{ 
	return (__n > (size_t)mp->max_bytes) ? _new(__n) : _pool_allocate(mp, __n); 
}

/* __p may not be 0 */
void deallocate(mempool_t mp, void *__p) 
{ 
	if (*(size_t *)((unsigned char *)__p - sizeof(size_t)) > (size_t)mp->max_bytes) _delete(__p);
	else _pool_deallocate(mp, __p, *(size_t *)((unsigned char *)__p - sizeof(size_t))); 
}


void shrink(mempool_t mp)
{
	union _obj *pobj = NULL, *pNext = NULL;
	int __i = 0; 

	if(NULL == mp) return;
	{
		if(mp->bmt) pthread_mutex_lock(&mp->lock);
		for(__i = 0; __i < _MEMPOOL_NFREELISTS; __i++)
		{
			pobj = mp->_free_list[__i];

			while (pobj != NULL)
			{
				pNext = pobj->free_list_link;
				_delete(pobj);
				pobj = pNext;
			}
		}
		if(mp->bmt) pthread_mutex_unlock(&mp->lock);
	}
}