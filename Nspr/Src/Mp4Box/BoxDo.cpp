#include "Nspr.h"

#include <stdarg.h>
#include <stdio.h>

namespace nspr
{

void NsprLog::LogMessage(int loglevel, const char *file, const char *func, int line, const char *format, ...)
{
    va_list args; 
    va_start(args, format);
    char logBuffer[4096] = {0};
    size_t len = 0;

    len += snprintf(logBuffer + len, sizeof(logBuffer) - 1 - len, "[%s.%d]: ", file, line);
    len += vsnprintf(logBuffer + len, sizeof(logBuffer) - 1 - len, format, args); 
    len += snprintf(logBuffer + len, sizeof(logBuffer) - 1 - len, "\n");
    va_end(args);
   
    nspr_app_core_write_log(&logBuffer[0], len);
}

}

void nspr_app_core_write_log(const char *logTxt, size_t logLen)
{
    printf("%s", logTxt);
}

