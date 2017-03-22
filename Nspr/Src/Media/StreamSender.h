#ifndef __NSPR_STREAM_SENDER_H_INCLUDED__
#define __NSPR_STREAM_SENDER_H_INCLUDED__

#include "StreamProc.h"

namespace nspr
{

class StreamConnection;
class StreamSenderProc : public StreamProc
{
public:
    StreamSenderProc(const u_char *addr);
    virtual ~StreamSenderProc();
    
    virtual int OnHandleTx(u_char *data, size_t len);
    virtual int OnHandleRx(u_char *data, size_t len);

	int GetLocalPort();

private:
    StreamConnection *m_sc;

}; // class PacketProc

} // namespace nspr

#endif // ifndef __NSPR_STREAM_SENDER_H_INCLUDED__

