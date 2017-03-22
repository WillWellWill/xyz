#include "Nspr.h"

#include "StreamConnection.h"
#include "StreamProc.h"

namespace nspr
{

StreamConnection::StreamConnection(const u_char *addr, StreamProc *proc)
    : m_c(nullptr)
	, m_proc(proc)
{
    snprintf((char *)m_url, sizeof(m_url), "udp://%s", (char *)addr);
    ngx_memzero(&m_recvBuf, sizeof(m_recvBuf));

    m_recvBuf.start = &m_rb[0];
    m_recvBuf.pos = m_recvBuf.last = m_recvBuf.start;
    m_recvBuf.end = m_recvBuf.start + m_maxRecvBufLen;
}

StreamConnection::~StreamConnection()
{
}
    
int StreamConnection::Connect()
{
    if (!nspr_callback_set.nspr_connect_peer_t) {
        return NSPR_ERROR;
    }
    m_c = (nspr_callback_set.nspr_connect_peer_t)(m_url);
    ngx_nspr_session_t *s = (ngx_nspr_session_t *)m_c->data;
    s->nspr_connection = this;
    return NSPR_OK;
}

ngx_buf_t *StreamConnection::GetSendBuffer()
{
    return nullptr;
}

void StreamConnection::ConfirmSendBuffer()
{
    return;
}

ngx_buf_t *StreamConnection::GetRecvBuffer()
{
    return &m_recvBuf;
}

void StreamConnection::ConfirmRecvBuffer()
{
	m_proc->OnHandleRx(m_recvBuf.pos, m_recvBuf.last - m_recvBuf.pos);
    m_recvBuf.last = m_recvBuf.pos;
    return;
}
    
int StreamConnection::Send(u_char *data, size_t len)
{
    sendto(m_c->fd, data, len, 0, m_c->sockaddr, m_c->socklen);
    return NSPR_OK;
}
	
int StreamConnection::GetLocalPort()
{
	if (!m_c || !m_c->local_sockaddr) {
		return NSPR_ERROR;
	}
    struct sockaddr_in *sin = (struct sockaddr_in *)m_c->local_sockaddr;
    return ntohs(sin->sin_port);
}

} // namespace StreamConnection

