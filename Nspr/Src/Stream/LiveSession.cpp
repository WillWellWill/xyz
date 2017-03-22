#include "Nspr.h"

#include "LiveSession.h"
#include "MediaBridge.h"
#include "Utility.h"

namespace nspr
{

LiveSession::LiveSession(const u_char *connId, Metadata *metadata)
	: m_metadata(metadata)
{
	memcpy(m_connId, connId, ngx_strlen(connId) + 1);
	Utility u;
	u.IpFromConnId((u_char *)connId, (u_char *)m_ip, sizeof(m_ip));
	NsprInfo("ip address: %s from %s", m_ip, connId);
	m_mediaBridge = new MediaBridge(m_ip);
}
	
int LiveSession::CreateStream()
{
	if (m_mediaBridge->BuildBridge(m_metadata)) {
		return NSPR_ERROR;
	}
	return NSPR_OK;
}

LiveSession::~LiveSession()
{
	delete m_mediaBridge;
}

} // namespace nspr

