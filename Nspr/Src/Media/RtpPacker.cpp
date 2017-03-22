#include "Nspr.h"

#include "RtpPacker.h"
#include "StreamConnection.h"

namespace nspr
{

RtpPacker::RtpPacker(StreamProc *proc)
    : m_proc(proc)
    , m_sequence(0)
    , m_startTimeMS(nspr_current_timems())
{
}

RtpPacker::~RtpPacker()
{
}

int RtpPacker::Pack(u_char *data, size_t len)
{
    u_char *raw_data = nullptr;
    size_t raw_data_len = 0;
    if (data[0] == 0 && data[1] == 0 && data[2] == 1) {
        raw_data = data + 3;
        raw_data_len = len - 3;
    }
    else if (data[0] == 0 && data[1] == 0 && data[2] == 0 && data[3] == 1) {
        raw_data = data + 4;
        raw_data_len = len - 4;
    }
    else {
        return NSPR_ERROR;
    }
    if (raw_data_len <= m_maxPacketLen) {
        return PackSingle(raw_data, raw_data_len);
    }

    return PackFua(raw_data, raw_data_len);
}

int RtpPacker::PackSingle(u_char *data, size_t len)
{
    u_char out[1600] = {0};
    int pointer = FillHeader(out);
    memcpy(&out[0] + pointer, data, len);
    Output(&out[0], len + pointer);
    return 0;
}

int RtpPacker::PackFua(u_char *data, size_t len)
{
    const u_char naluByte = *data;
    const int win = m_maxPacketLen - 1;

    bool isFirst = true;
    size_t const payloadLen = len - 1;
    u_char *payload = data + 1;
    int packedBytes = win;
    for (; packedBytes < (int)payloadLen; packedBytes += win) {
        PackOneFua(isFirst, false, naluByte, payload + packedBytes - win, win);
        if (isFirst) {
            isFirst = false;
        }
    }
    PackOneFua(false, true, naluByte, payload + packedBytes - win, payloadLen - (packedBytes - win));
    return NSPR_OK;
}

int RtpPacker::PackOneFua(bool isFirst, bool isLast, u_char naluByte, u_char *data, size_t len)
{
    const u_char ser = (isFirst || isLast) ? (isFirst ? 0x80 : 0x40) : 0x0;
    u_char out[1600] = {0};
    int pointer = FillHeader(out);
    out[pointer++] = (naluByte & 0xe0) | 0x1c;
    out[pointer++] = ser | (naluByte & 0x1f);
    memcpy(&out[0] + pointer, data, len);
    Output(&out[0], len + pointer);
    return NSPR_OK;
}

int RtpPacker::FillHeader(u_char *pkt)
{
    pkt[0] = 0x80;
    pkt[1] = 0x11; // payload type
    pkt[2] = ((u_char *)&m_sequence)[1];
    pkt[3] = ((u_char *)&m_sequence)[0];
    uint64_t cur = nspr_current_timems() - m_startTimeMS;
    pkt[4] = ((uint32_t *)&cur)[3];
    pkt[5] = ((uint32_t *)&cur)[2];
    pkt[6] = ((uint32_t *)&cur)[1];
    pkt[7] = ((uint32_t *)&cur)[0];
    ngx_memzero(pkt + 8, 4);
    return 12;
}

void RtpPacker::Output(u_char *pkt, size_t pktLen)
{
    m_proc->RelayTx(pkt, pktLen);
    m_sequence++;
}

RtpPackerProc::RtpPackerProc()
    : m_rp(new RtpPacker(this))
{
}

RtpPackerProc::~RtpPackerProc()
{
    delete m_rp;
}

int RtpPackerProc::OnHandleTx(u_char *data, size_t len)
{
    return m_rp->Pack(data, len);
}

int RtpPackerProc::OnHandleRx(u_char *data, size_t len)
{
	(void)data; (void)len;
    return NSPR_ERROR;
}

} // namespace nspr

