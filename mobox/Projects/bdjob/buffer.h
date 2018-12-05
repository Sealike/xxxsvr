#ifndef _SX_BUF_H
#define _SX_BUF_H

#ifdef __cplusplus
extern "C"
{
#endif
/** prototype for the write notify function */
typedef void (*_sx_notify_t)(void *s, void *arg);

/** utility: buffer */
typedef struct _sx_buf_st *sx_buf_t;
struct _sx_buf_st {
	unsigned char           *data;     /* pointer to buffer's data */
	unsigned int            len;       /* length of buffer's data */
	unsigned char           *heap;     /* beginning of malloc() block containing data, if non-NULL */

	/* function to call when this buffer gets written */
	_sx_notify_t            notify;
	void                    *notify_arg;
};

/* buffer utilities */
sx_buf_t  _sx_buffer_new(const char *data, int len, _sx_notify_t notify, void *notify_arg);
void      _sx_buffer_free(sx_buf_t buf);
void      _sx_buffer_clear(sx_buf_t buf);
void      _sx_buffer_alloc_margin(sx_buf_t buf, int before, int after);
void 	  _sx_buffer_append(sx_buf_t buf, const char * data, int len);
void      _sx_buffer_set(sx_buf_t buf, char *newdata, int newlength, char *newheap);
#ifdef __cplusplus
}
#endif
#endif