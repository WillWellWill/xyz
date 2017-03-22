#ifndef __NSPR_INTERACT_STREAM_PROC_H_INCLUDED_H__
#define __NSPR_INTERACT_STREAM_PROC_H_INCLUDED_H__

#include "StreamProc.h"

namespace nspr
{

class InteractStreamProc : public StreamProc
{
public:
    InteractStreamProc();
    virtual ~InteractStreamProc();

    virtual int OnHandleTx(u_char *data, size_t len);
    virtual int OnHandleRx(u_char *data, size_t len);
    
private:
}; // class InteractStreamProc

} // namespace nspr

#endif // ifndef __NSPR_INTERACT_STREAM_PROC_H_INCLUDED_H__

