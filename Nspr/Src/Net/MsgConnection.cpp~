#include "Nspr.h"

#include "MsgConnection.h"

namespace nspr
{

MsgConnection::MsgConnection(const u_char *url)
    : m_c(nullptr)
{
    memcpy(m_url, url, ngx_strlen(url) + 1);
    ngx_queue_init(&m_sendQueue);
    ngx_memzero(&m_recvBuf, sizeof(m_recvBuf));
    m_recvBuf.pos = m_recvBuf.last = m_recvBuf.start = new u_char[m_maxRecvBufLen];
    m_recvBuf.end = m_recvBuf.start + m_maxRecvBufLen;
}

MsgConnection::MsgConnection(ngx_connection_t *c)
    : m_c(c)
{
    NsprInfo("connection in...");
    ngx_queue_init(&m_sendQueue);
    ngx_memzero(&m_recvBuf, sizeof(m_recvBuf));
    m_recvBuf.pos = m_recvBuf.last = m_recvBuf.start = new u_char[m_maxRecvBufLen];
    m_recvBuf.end = m_recvBuf.start + m_maxRecvBufLen;
}

MsgConnection::~MsgConnection()
{
    NsprInfo("close...");
    delete []m_recvBuf.pos;
}

int MsgConnection::Connect()
{
    if (!nspr_callback_set.nspr_connect_peer_t) {
        return NSPR_ERROR;
    }
	NsprInfo("to connect peer %s", m_url);
    m_c = (nspr_callback_set.nspr_connect_peer_t)(m_url);
    ngx_nspr_session_t *s = (ngx_nspr_session_t *)m_c->data;
    s->nspr_connection = this;
    return NSPR_OK;
}

ngx_buf_t *MsgConnection::GetSendBuffer()
{
    NsprDebug("get send buffer");
    if (ngx_queue_empty(&m_sendQueue)) {
        return nullptr;
    }
    NsprDebug("get send buffer not empty");
    MsgParcel *mp = ngx_queue_data(ngx_queue_head(&m_sendQueue), MsgParcel, queue);
    return &mp->buf;
}

void MsgConnection::ConfirmSendBuffer()
{
    MsgParcel *mp = ngx_queue_data(ngx_queue_head(&m_sendQueue), MsgParcel, queue);
    if (mp->buf.last == mp->buf.pos) {
        ngx_queue_remove(&mp->queue);
		NsprDebug("send complete");
        delete []mp->buf.start;
        delete mp;
    }
    if (!ngx_queue_empty(&m_sendQueue) && nspr_callback_set.nspr_post_ioevent_t) {
        //(nspr_callback_set.nspr_post_ioevent_t)(m_c->write);
        m_c->write->handler(m_c->write);
    }
    return;
}

ngx_buf_t *MsgConnection::GetRecvBuffer()
{
    return &m_recvBuf;
}

void MsgConnection::ConfirmRecvBuffer()
{
    ngx_buf_t &b = m_recvBuf;
    do {
        int ret = OnRecv(b.pos, b.last - b.pos);
        if (NSPR_AGAIN == ret) {
            return;
        }
        else if (NSPR_ERROR == ret || NSPR_OK == ret) {
            b.pos = b.last = b.start;
            return;
        }
        else if (ret > 0) {
            b.pos += ret;
            if (b.pos >= b.last) {
                b.pos = b.last = b.start;
                return;
            }
        }
    } while(true);
    return;
}
    
int MsgConnection::Send(const u_char *msg, size_t len)
{
    if (!nspr_callback_set.nspr_post_ioevent_t) {
        return NSPR_ERROR;
    }
    MsgParcel *mp = new MsgParcel();
    ngx_buf_t &b = mp->buf;
    ngx_memzero(&b, sizeof(b));
    b.start = new u_char[len];
    memcpy(b.start, msg, len);
    b.pos = b.start;
    b.last = b.end = b.start + len;
    ngx_queue_insert_tail(&m_sendQueue, &mp->queue);
    NsprDebug("sending %d", len);
    // (nspr_callback_set.nspr_post_ioevent_t)(m_c->write);
    m_c->write->handler(m_c->write);
    return NSPR_OK;
}

int MsgConnection::OnRecv(const u_char *msg, size_t len)
{
    return NSPR_OK;
}

void MsgConnection::Close()
{
    if (m_c && (nspr_callback_set.nspr_post_ioevent_t)) {
        ngx_nspr_session_t *s = (ngx_nspr_session_t *)m_c->data;
        NsprInfo("close event");
        (nspr_callback_set.nspr_post_ioevent_t)(&s->close); 
    }
}

} // namespace MsgConnection



