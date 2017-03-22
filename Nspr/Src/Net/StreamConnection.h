#ifndef __NSPR_STREAM_CONNECTION__
#define __NSPR_STREAM_CONNECTION__

namespace nspr
{

class StreamProc;
class StreamConnection
{
public:
    StreamConnection(const u_char *addr, StreamProc *proc);
    virtual ~StreamConnection();

    virtual int Connect();

    ngx_buf_t *GetSendBuffer();
    void ConfirmSendBuffer();
    ngx_buf_t *GetRecvBuffer();
    void ConfirmRecvBuffer();

    virtual int Send(u_char *data, size_t len);

	int GetLocalPort();

private:
    static const int m_maxUrlLen = 1024;
    static const int m_maxRecvBufLen = 4096;
    
    u_char m_url[m_maxUrlLen];
    ngx_buf_t m_recvBuf;
    u_char m_rb[m_maxRecvBufLen];

    ngx_connection_t *m_c;
	StreamProc *m_proc;
}; // class StreamConnection

} // namespace nspr


#endif // __NSPR_STREAM_CONNECTION__

