#ifndef __NSPR_MSG_CONNECTION__
#define __NSPR_MSG_CONNECTION__

namespace nspr
{

class MsgConnection
{
public:
    MsgConnection(const u_char *url);
    MsgConnection(ngx_connection_t *c);
    virtual ~MsgConnection();

    virtual int Connect();

    ngx_buf_t *GetSendBuffer();
    void ConfirmSendBuffer();
    ngx_buf_t *GetRecvBuffer();
    void ConfirmRecvBuffer();

    int Send(const u_char *msg, size_t len);
    virtual int OnRecv(const u_char *msg, size_t len);

    virtual void Close();

private:
    static const int m_maxUrlLen = 1024;
    static const int m_maxRecvBufLen = 4096;
    u_char m_url[m_maxUrlLen];

    ngx_connection_t *m_c;
    ngx_queue_t m_sendQueue;
    ngx_buf_t m_recvBuf;
    
    struct MsgParcel
    {
        ngx_buf_t    buf;
        ngx_queue_t  queue;
    };

}; // class MsgConnection

} // namespace nspr


#endif // __NSPR_MSG_CONNECTION__

