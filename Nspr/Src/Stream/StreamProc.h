#ifndef __NSPR_STREAM_PROC_H_INCLUDED_H__
#define __NSPR_STREAM_PROC_H_INCLUDED_H__

#include <list>

namespace nspr
{

class StreamProc
{
public:
    StreamProc() : m_txChannels(), m_rxChannel(nullptr)
    {}
    virtual ~StreamProc() {}

    virtual int OnHandleTx(u_char *data, size_t len) = 0;
    virtual int OnHandleRx(u_char *data, size_t len) = 0;

    virtual int RelayTx(u_char *data, size_t len);
    virtual int RelayRx(u_char *data, size_t len) { if (m_rxChannel) { return m_rxChannel->OnHandleRx(data, len); } return NSPR_ERROR; }

    int AttachTxChannel(StreamProc *txChannel);
    int DetachTxChannel(StreamProc *txChannel);
    int AttachRxChannel(StreamProc *rxChannel);
    int DetachRxChannel(StreamProc *rxChannel);

    virtual int AttachedTxBy(StreamProc *proc) { (void)proc; return NSPR_OK; }
    virtual int DetachedTxBy(StreamProc *proc) { (void)proc; return NSPR_OK; }
    virtual int AttachedRxBy(StreamProc *proc) { (void)proc; return NSPR_OK; }
    virtual int DetachedRxBy(StreamProc *proc) { (void)proc; return NSPR_OK; }

    virtual bool IsTextMode() { return false; }

private:
    std::list<StreamProc *> m_txChannels;
    StreamProc *m_rxChannel;
}; // class StreamProc

class EmptyStreamProc : public StreamProc
{
public:
    int OnHandleTx(u_char *data, size_t len) { return NSPR_OK; }
    int OnHandleRx(u_char *data, size_t len) { (void)data; (void)len; return NSPR_OK; }
}; // class StreamProc

} // namespace nspr

#endif // ifndef __NSPR_STREAM_PROC_H_INCLUDED_H__

