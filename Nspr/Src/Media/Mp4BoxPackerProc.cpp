#include "Nspr.h"

#include "Mp4BoxPackerProc.h"
#include "BoxPacker.h"

namespace nspr
{

Mp4BoxPackerProc::Mp4BoxPackerProc(VideoCodec::Parameter *parameter)
    : m_boxPacker(new BoxPacker(parameter))
    , m_maxBoxLen(parameter->GetBitrate() * 5/*seconds*/ / 8 + 1024)
    , m_hasInjectFrame(false)
{
    m_box = new u_char[m_maxBoxLen];
    m_boxLen = 0;
}

Mp4BoxPackerProc::~Mp4BoxPackerProc()
{
    delete []m_box;
    delete m_boxPacker;
}
    
void Mp4BoxPackerProc::Reset()
{
    VideoCodec::Parameter *param = m_boxPacker->GetParameter();
    delete m_boxPacker;
    m_boxPacker = new BoxPacker(param);
    m_hasInjectFrame = false;
    m_boxLen = 0;
}

bool Mp4BoxPackerProc::IsPackingTime(u_char *data, size_t len)
{
    if (!m_hasInjectFrame){
        return false;
    }
    return true;
}

int Mp4BoxPackerProc::OnHandleTx(u_char *data, size_t len)
{
    if (IsPackingTime(data, len)) {
        m_boxLen = m_maxBoxLen;
        if (m_boxPacker->Pack(m_box, m_boxLen)) {
            return NSPR_ERROR;
        }
        if (RelayTx(m_box, m_boxLen)) {
            return NSPR_ERROR;
        }
    }
    if (m_boxPacker->InjectFrame(data, len)) {
		return NSPR_ERROR;
	}
    m_hasInjectFrame = true;

	return NSPR_OK;
}

int Mp4BoxPackerProc::OnHandleRx(u_char *data, size_t len)
{
	return NSPR_OK;
}

int Mp4BoxPackerProc::AttachedTxBy(StreamProc *proc)
{
	return NSPR_OK;
}

} // namespace nspr

