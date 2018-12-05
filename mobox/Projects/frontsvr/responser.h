#ifndef __responser_h_
#define __responser_h_

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include "util/inaddr.h"
#include "ac-stdint.h"

/* jabberd2 Windows DLL */
#ifndef JABBERD2_API
# ifdef _WIN32
#  ifdef JABBERD2_EXPORTS
#   define JABBERD2_API  __declspec(dllexport)
#  else /* JABBERD2_EXPORTS */
#   define JABBERD2_API  __declspec(dllimport)
#  endif /* JABBERD2_EXPORTS */
# else /* _WIN32 */
#  define JABBERD2_API extern
# endif /* _WIN32 */
#endif /* JABBERD2_API */

#include <expat.h>
#include <util/util.h>
#include <mio/mio.h>
#include <pthreads/pthread.h>
#include "client.h"

#ifdef HAVE_SIGNAL_H
# include <signal.h>
#endif
#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*responser_callback_t)(void *result, int resultl, void *arg);

int responser_mio_callback(mio_t m, mio_action_t a, mio_fd_t fd, void *data, void *arg); 

struct _responser_st {
	responser_callback_t pop_data_fun;
	void * pop_data_para;

	client_p  client;

	char send_buff[64 * 1024];

	char recv_buff[64 * 1024];

	/* 命令通道服务,UDP模式 */
	char *local; 	/* 默认 127.0.0.1,目的是只让本地访问 */
	int local_port;	/* 默认 0, 目的是让系统随机设置端口 */
	mio_fd_t recv_fd;

	mio_fd_t send_fd;
	sig_atomic_t shutdown;

	int    measure_mode;
	time_t measure_interval;
	time_t last_measure;
};

typedef struct _responser_st		*responser_t;
typedef struct _responser_st		responser_st;

JABBERD2_API responser_t responser_new(config_t config, responser_callback_t data_cb, void *data_cb_para, client_p client, long cid) ;
JABBERD2_API void responser_free(responser_t processor);

void responser_write(responser_t processor, const char *data, int datal);

#ifdef __cplusplus
}
#endif

#endif /*#define __processor_h */
