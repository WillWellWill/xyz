#include "Nspr.h"

#include "NsprLog.h"

namespace nspr
{

void NsprLog::LogMessage(int loglevel, const char *file, const char *func, int line, const char *format, ...)
{
#if 1
    if (loglevel == NSPR_LOG_DEBUG1) {
        return;
    }
#endif
    va_list args; 
    va_start(args, format);
    char logBuffer[4096] = {0};
    size_t len = 0;

    struct timeval tv;
    gettimeofday(&tv, 0);
    struct tm *lt = localtime(&tv.tv_sec);
    len += snprintf(logBuffer + len, sizeof(logBuffer) - 1 - len, "[%d:%d:%d.%ld][%s.%d]: "
                                    , lt->tm_hour, lt->tm_min, lt->tm_sec, (long)tv.tv_usec / 1000
                                    , file, line);
    len += vsnprintf(logBuffer + len, sizeof(logBuffer) - 1 - len, format, args); 
    len += snprintf(logBuffer + len, sizeof(logBuffer) - 1 - len, "\n");
    va_end(args);
   
    nspr_app_core_write_log(&logBuffer[0], len);
}

}

