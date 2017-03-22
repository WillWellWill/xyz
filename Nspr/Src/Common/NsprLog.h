#ifndef __NSPR_LOG_INCLUDED_H__
#define __NSPR_LOG_INCLUDED_H__


namespace nspr
{

#define NSPR_LOG_DEBUG1 99
#define NSPR_LOG_DEBUG 100
#define NSPR_LOG_INFO 101
#define NSPR_LOG_WARN 102
#define NSPR_LOG_ERROR 103

class NsprLog
{
public:
    static void LogMessage(int loglevel, const char *file, const char *func, int line, const char *format, ...);

}

#define NsprLog(loglevel, format, args...) NsprLog::LogMessage(loglevel, __FILE__, __FUNCTION__, __LINE__, format, ##args)
#define NsprDebug(format, args...) NsprLog(NSPR_LOG_DEBUG, format, ##args)
#define NsprInfo(format, args...) NsprLog(NSPR_LOG_INFO, format, ##args)
#define NsprWarn(format, args...) NsprLog(NSPR_LOG_WARN, format, ##args)
#define NsprError(format, args...) NsprLog(NSPR_LOG_ERROR, format, ##args)

; // class MessageBuffer

} // namespace Interact


#endif // ifndef __NSPR_LOG_INCLUDED_H__

