#include "Nspr.h"

#include "MsgConnection.h"
#include "WSConnection.h"
#include "ApiDispatcher.h"
#include "StreamProcFactory.h"

using namespace nspr;

extern "C" void nspr_app_core_api_init();
extern "C" ngx_int_t nspr_app_core_api_process(ngx_http_request_t *r, nspr_http_response_cb cb);

extern "C" void nspr_app_core_tcp_create_connection(ngx_connection_t *c);
extern "C" void nspr_app_core_tcp_destroy_connection(ngx_connection_t *c);
extern "C" void nspr_app_core_set_connect_peer_cb(nspr_connect_peer_cb cb);

extern "C" ngx_buf_t *nspr_app_core_get_send_buffer(ngx_nspr_session_t *session);
extern "C" void nspr_app_core_confirm_send_buffer(ngx_nspr_session_t *session);
extern "C" ngx_buf_t *nspr_app_core_get_recv_buffer(ngx_nspr_session_t *session);
extern "C" void nspr_app_core_confirm_recv_buffer(ngx_nspr_session_t *session);
extern "C" void nspr_app_core_set_callback(ngx_nspr_callback_t cb);

extern "C" void nspr_app_core_udp_send(ngx_event_t *wev);
extern "C" void nspr_app_core_udp_recv(ngx_event_t *rev);

ngx_nspr_callback_t nspr_callback_set = {nullptr, nullptr};

void nspr_app_core_api_init()
{
	NsprInfo("app core api init, %d", ngx_getpid());
}

ngx_int_t nspr_app_core_api_process(ngx_http_request_t *r, nspr_http_response_cb cb)
{
    return Singleton<ApiDispatcher>::GetInstance()->OnRequest(r, cb);
}

void nspr_app_core_tcp_create_connection(ngx_connection_t *c)
{
    ngx_nspr_session_t *s = (ngx_nspr_session_t *)c->data;
    s->nspr_connection = new WSConnection(c);
}

void nspr_app_core_tcp_destroy_connection(ngx_connection_t *c)
{
    ngx_nspr_session_t *s = (ngx_nspr_session_t *)c->data;
    delete (WSConnection *)s->nspr_connection;
}

extern "C" void nspr_app_core_log(const char *logTxt, size_t logLen)
{
    nspr_app_core_write_log(logTxt, logLen);
}

void nspr_app_core_write_log(const char *logTxt, size_t logLen)
{
    
    FILE *fp = fopen("nspr.log", "a+");
    fwrite((char *)logTxt, 1, logLen, fp);
    fclose(fp);
}

void nspr_app_core_set_callback(ngx_nspr_callback_t cb)
{
    nspr_callback_set = cb;
}

ngx_buf_t *nspr_app_core_get_send_buffer(ngx_nspr_session_t *session)
{
    MsgConnection *mc = (MsgConnection *)session->nspr_connection;
    return mc->GetSendBuffer();
}

void nspr_app_core_confirm_send_buffer(ngx_nspr_session_t *session)
{
    MsgConnection *mc = (MsgConnection *)session->nspr_connection;
    mc->ConfirmSendBuffer();
}

ngx_buf_t *nspr_app_core_get_recv_buffer(ngx_nspr_session_t *session)
{
    MsgConnection *mc = (MsgConnection *)session->nspr_connection;
    return mc->GetRecvBuffer();
}

void nspr_app_core_confirm_recv_buffer(ngx_nspr_session_t *session)
{
    MsgConnection *mc = (MsgConnection *)session->nspr_connection;
    return mc->ConfirmRecvBuffer();
}

void nspr_app_core_udp_send(ngx_event_t *wev)
{
}

void nspr_app_core_udp_recv(ngx_event_t *rev)
{
    ssize_t                     n;
    ngx_connection_t           *c; 

    c = (ngx_connection_t *)rev->data;

    struct sockaddr_in *sin = (struct sockaddr_in *)c->local_sockaddr;
    const int srcPort = ntohs(sin->sin_port);
    StreamProc *proc = Singleton<StreamProcFactory>::GetInstance()->GetStreamProc(srcPort, "");

    do {
        struct sockaddr addr;
        socklen_t addrlen = NGX_SOCKADDR_STRLEN;
        u_char data[1600];
        n = recvfrom(c->fd, data, sizeof(data), 0, &addr, &addrlen);
        if (n <= 0) {
            break;
        }
        u_char text[NGX_SOCKADDR_STRLEN + 1] = {0};

        sin = (struct sockaddr_in *) &addr;
        u_char *p = (u_char *) &sin->sin_addr;
        snprintf((char *)text, sizeof(text), "%u.%u.%u.%u:%d",
                             p[0], p[1], p[2], p[3], ntohs(sin->sin_port));

        if (proc) {
            proc->OnHandleRx(data, n);
        }
        //NsprDebug("udp receive %d bytes from %s", n, text);

    } while (n > 0);
}

