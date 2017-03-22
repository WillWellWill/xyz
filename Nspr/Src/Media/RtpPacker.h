#ifndef __NSPR_RTP_PACKER_H_INCLUDED__
#define __NSPR_RTP_PACKER_H_INCLUDED__

#include "StreamProc.h"

namespace nspr
{

class RtpPacker
{
public:
    RtpPacker(StreamProc *proc);
    ~RtpPacker();

    int Pack(u_char *data, size_t len);

private:
    const static int m_maxPacketLen = 1140;
    StreamProc *m_proc;
    uint16_t m_sequence;
    uint64_t m_startTimeMS;

private:
    int PackSingle(u_char *data, size_t len);
    int PackFua(u_char *data, size_t len);
    int PackOneFua(bool isFirst, bool isLast, u_char naluByte, u_char *data, size_t len);
    int FillHeader(u_char *pkt);
    void Output(u_char *pkt, size_t pktLen);

}; // class RtpPacker

class RtpPackerProc : public StreamProc
{
public:
    RtpPackerProc();
    ~RtpPackerProc();

    virtual int OnHandleTx(u_char *data, size_t len);
    virtual int OnHandleRx(u_char *data, size_t len);

private:
    RtpPacker *m_rp;

}; // class RtpPackerProc

} // namespace nspr

#endif // ifndef __NSPR_RTP_PACKER_H_INCLUDED__

