#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#if !defined(HAVE_INET_PTON) && defined(_WIN32)
#ifdef JABBERD2_EXPORTS
# define JABBERD2_API  __declspec(dllexport)
#else /* JABBERD2_EXPORTS */
# define JABBERD2_API  __declspec(dllimport)
#endif /* JABBERD2_EXPORTS */

#include "ac-stdint.h"

#ifdef HAVE_WINSOCK2_H
# include <winsock2.h>
#ifdef HAVE_IP_V6
# include "ip6_misc.h"
#endif
#endif

#include <errno.h>

#ifndef EAFNOSUPPORT
#define	EAFNOSUPPORT	97	/* not present in errno.h provided with VC */
#endif

JABBERD2_API int
inet_pton(int af, const char *src, void *dst)
{
    if (af != AF_INET) {
	errno = EAFNOSUPPORT;
	return -1;
    }
    return inet_aton (src, dst);
}

#endif
