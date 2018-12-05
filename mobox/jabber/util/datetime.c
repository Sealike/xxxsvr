#include "util/util.h"
#include <sys/timeb.h>
/* ISO 8601 / JEP-0082 date/time manipulation */

/* formats */
#define DT_DATETIME_P       "%04d-%02d-%02dT%02d:%02d:%lf+%02d:%02d"
#define DT_DATETIME_M       "%04d-%02d-%02dT%02d:%02d:%lf-%02d:%02d"
#define DT_DATETIME_Z       "%04d-%02d-%02dT%02d:%02d:%lfZ"
#define DT_TIME_P           "%02d:%02d:%lf+%02d:%02d"
#define DT_TIME_M           "%02d:%02d:%lf-%02d:%02d"
#define DT_TIME_Z           "%02d:%02d:%lfZ"
#define DT_LEGACY           "%04d%02d%02dT%02d:%02d:%lf"
#define DT_EXTEND           "%04d-%02d-%02d %02d:%02d:%lf"
#define DT_SHORTDT          "%04d%02d%02d%02d%02d%02d"

typedef long off_t; //NON-ANSI for compatibility

time_t datetime_in(char *date) {
    struct tm gmt, off;
    double sec;
    off_t fix = 0;
    struct timeval tv;
    struct timezone tz;
	time_t ret;
    assert((int) date);

    /* !!! sucks having to call this each time */
    tzset();

    memset(&gmt, 0, sizeof(struct tm));
    memset(&off, 0, sizeof(struct tm));

    if(sscanf(date, DT_DATETIME_P,
                   &gmt.tm_year, &gmt.tm_mon, &gmt.tm_mday,
                   &gmt.tm_hour, &gmt.tm_min, &sec,
                   &off.tm_hour, &off.tm_min) == 8) {
        gmt.tm_sec = (int) sec;
        gmt.tm_year -= 1900;
        gmt.tm_mon--;
        fix = off.tm_hour * 3600 + off.tm_min * 60;
    }

    else if(sscanf(date, DT_DATETIME_M,
                   &gmt.tm_year, &gmt.tm_mon, &gmt.tm_mday,
                   &gmt.tm_hour, &gmt.tm_min, &sec,
                   &off.tm_hour, &off.tm_min) == 8) {
        gmt.tm_sec = (int) sec;
        gmt.tm_year -= 1900;
        gmt.tm_mon--;
        fix = - off.tm_hour * 3600 - off.tm_min * 60;
    }

    else if(sscanf(date, DT_DATETIME_Z,
                   &gmt.tm_year, &gmt.tm_mon, &gmt.tm_mday,
                   &gmt.tm_hour, &gmt.tm_min, &sec) == 6) {
        gmt.tm_sec = (int) sec;
        gmt.tm_year -= 1900;
        gmt.tm_mon--;
        fix = 0;
    }

    else if(sscanf(date, DT_TIME_P,
                   &gmt.tm_hour, &gmt.tm_min, &sec,
                   &off.tm_hour, &off.tm_min) == 5) {
        gmt.tm_sec = (int) sec;
        fix = off.tm_hour * 3600 + off.tm_min * 60;
    }

    else if(sscanf(date, DT_TIME_M,
                   &gmt.tm_hour, &gmt.tm_min, &sec,
                   &off.tm_hour, &off.tm_min) == 5) {
        gmt.tm_sec = (int) sec;
        fix = - off.tm_hour * 3600 - off.tm_min * 60;
    }

    else if(sscanf(date, DT_TIME_Z,
                   &gmt.tm_hour, &gmt.tm_min, &sec) == 3) {
        gmt.tm_sec = (int) sec;
        fix = - off.tm_hour * 3600 - off.tm_min * 60;
    }

    else if(sscanf(date, DT_LEGACY,
                   &gmt.tm_year, &gmt.tm_mon, &gmt.tm_mday,
                   &gmt.tm_hour, &gmt.tm_min, &sec) == 6) {
        gmt.tm_sec = (int) sec;
        gmt.tm_year -= 1900;
        gmt.tm_mon--;
        fix = 0;
    }
	else if(sscanf(date, DT_EXTEND,
		&gmt.tm_year, &gmt.tm_mon, &gmt.tm_mday,
		&gmt.tm_hour, &gmt.tm_min, &sec) == 6) {
			gmt.tm_sec = (int) sec;
			gmt.tm_year -= 1900;
			gmt.tm_mon--;
			fix = 0;
	}
	else if(sscanf(date, DT_SHORTDT,
		&gmt.tm_year, &gmt.tm_mon, &gmt.tm_mday,
		&gmt.tm_hour, &gmt.tm_min, &gmt.tm_sec) == 6) {
			gmt.tm_year -= 1900;
			gmt.tm_mon--;
			fix = 0;
	}
    gmt.tm_isdst = -1;

    gettimeofday(&tv, &tz);
    return mktime(&gmt) + fix/* - (tz.tz_minuteswest * 60)*/;
}

void datetime_out(time_t t, datetime_t type, char *date, int datelen) {
    struct tm *gmt, *lmt;

    assert((int) type);
    assert((int) date);
    assert((int) datelen);

    gmt = gmtime(&t);
	lmt = localtime(&t);
    switch(type) {
        case dt_DATE:
            snprintf(date, datelen, "%04d-%02d-%02d", gmt->tm_year + 1900, gmt->tm_mon + 1, gmt->tm_mday);
            break;
        case dt_TIME:
            snprintf(date, datelen, "%02d:%02d:%02dZ", gmt->tm_hour, gmt->tm_min, gmt->tm_sec);
            break;
        case dt_DATETIME:
            snprintf(date, datelen, "%04d-%02d-%02dT%02d:%02d:%02dZ", gmt->tm_year + 1900, gmt->tm_mon + 1, gmt->tm_mday, gmt->tm_hour, gmt->tm_min, gmt->tm_sec);
            break;
        case dt_LEGACY:
            snprintf(date, datelen, "%04d%02d%02dT%02d:%02d:%02d", gmt->tm_year + 1900, gmt->tm_mon + 1, gmt->tm_mday, gmt->tm_hour, gmt->tm_min, gmt->tm_sec);
            break;
		case dt_LDATETIME:
			snprintf(date, datelen, "%04d-%02d-%02d %02d:%02d:%02d", lmt->tm_year + 1900, lmt->tm_mon + 1, lmt->tm_mday, lmt->tm_hour, lmt->tm_min, lmt->tm_sec);
			break;
		case dt_SHORTDT:
			snprintf(date, datelen, "%04d%02d%02d%02d%02d%02d", gmt->tm_year + 1900, gmt->tm_mon + 1, gmt->tm_mday, gmt->tm_hour, gmt->tm_min, gmt->tm_sec);
			break;
    }
}

#ifdef _WIN32
unsigned long long UNGetTickCount()
{
	return GetTickCount();
}
#else /* LINUX */
unsigned long long UNGetTickCount()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC,&ts);                      //此处可以判断一下返回值
	return (ts.tv_sec*1000 + ts.tv_nsec/(1000*1000));
}
#endif 

#ifdef _WIN32
int64_t gettickcount() 
{
	struct __timeb64 now;
	_ftime64(&now);
	return now.time * 1000 - now.timezone * 60 * 1000 + now.millitm;
}
#else /* LINUX */
int64_t gettickcount() 
{
	struct timeval now;
	gettimeofday(&now, NULL); 
	return ((int64_t)now.tv_sec) * 1000 + (int64_t)now.tv_usec/1000;
}
#endif 