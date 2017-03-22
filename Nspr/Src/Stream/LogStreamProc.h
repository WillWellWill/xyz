#ifndef __NSPR_LOG_STREAM_PROC_H_INCLUDED_H__
#define __NSPR_LOG_STREAM_PROC_H_INCLUDED_H__

#include "StreamProc.h"

namespace nspr
{

class LogStreamProc : public StreamProc
{
public:
    LogStreamProc();
    virtual ~LogStreamProc();

    virtual int OnHandleTx(u_char *data, size_t len);
    virtual int OnHandleRx(u_char *data, size_t len);
    
    virtual bool IsTextMode() { return true; }

private:
}; // class LogStreamProc

} // namespace nspr

#endif // ifndef __NSPR_LOG_STREAM_PROC_H_INCLUDED_H__

