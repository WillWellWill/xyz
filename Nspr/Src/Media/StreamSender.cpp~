#include "Nspr.h"

#include "StreamSender.h"
#include "StreamConnection.h"

namespace nspr
{

StreamSenderProc::StreamSenderProc(const u_char *addr)
    : m_sc(new StreamConnection(addr, this))
{
    m_sc->Connect();
}

StreamSenderProc::~StreamSenderProc()
{
    delete m_sc;
}

int StreamSenderProc::OnHandleTx(u_char *data, size_t len)
{
    m_sc->Send(data, len + 4);
    return NSPR_OK;
}

int StreamSenderProc::OnHandleRx(u_char *data, size_t len)
{
	//NsprDebug("len = %d", len);
	if (RelayRx(data, len - 4)) {
		return NSPR_ERROR;
	}
    return NSPR_OK;
}
	
int StreamSenderProc::GetLocalPort()
{
	return m_sc->GetLocalPort();
}

} // namespace nspr

