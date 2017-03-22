#ifndef __NSPR_NSPR_H__
#define __NSPR_NSPR_H__

#ifndef NGINX_OFF
#ifdef __cplusplus
extern "C" {
#endif
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>
#include <nginx.h>
#include <ngx_http.h>
#include <ngx_nspr.h>
#include <sqlite3.h>
#ifdef __cplusplus
}
#endif
#else
typedef unsigned char u_char;
#endif
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <string>

#include "NsprLog.h"

#define nullptr 0

const static int NSPR_OK = 0;
const static int NSPR_ERROR = -1;
const static int NSPR_AGAIN = -8;

void nspr_app_core_write_log(const char *logTxt, size_t logLen);

inline uint64_t nspr_current_timems() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return (uint64_t)tv.tv_sec * 1000 + (uint64_t)tv.tv_usec / 1000; 
}

#include "Singleton.h"

#endif // __NSPR_NSPR_H__

