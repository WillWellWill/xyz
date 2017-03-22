#ifndef __NSPR_API_DISPATCHER_H_INCLUDED__
#define __NSPR_API_DISPATCHER_H_INCLUDED__

#include "ProtocolConnection.h"

namespace nspr
{
class ApiDispatcher;
class LiveSession;
class StatusResponse : public ProtocolStatus
{
public:
    StatusResponse(ApiDispatcher *api, ngx_http_request_t *r, nspr_http_response_cb cb);
    virtual void OnConnected(const u_char *connId, Metadata *metadata);
    virtual void OnDestroyed(const u_char *connId);
    virtual void OnReady(const u_char *connId);

private:
    //ApiDispatcher *m_api;
    //ngx_http_request_t *m_r;
    //nspr_http_response_cb m_cb;
	LiveSession *m_liveSession;

}; // class StatusResponse
class ApiDispatcher
{
public:
    ApiDispatcher();
    ~ApiDispatcher();

    int OnRequest(ngx_http_request_t *r, nspr_http_response_cb cb);
    void LogCB(const u_char *logTxt, size_t logLen);
    
public:
    friend class StatusResponse;

private:
    const static int m_maxResponseStrLen = 64 * 1024;
    u_char m_responseStr[m_maxResponseStrLen];
    ngx_chain_t *m_response;
    
private:
    int DispatchPublish(ngx_http_request_t *req, nspr_http_response_cb cb);
    int DispatchPlay(ngx_http_request_t *req, nspr_http_response_cb cb);
	int DispatchQuery(ngx_http_request_t *req, nspr_http_response_cb cb);
	int DispatchGetAnotherWord(ngx_http_request_t *r, nspr_http_response_cb cb);

    int DispatchConnect(ngx_http_request_t *r, nspr_http_response_cb cb, bool isPublish);
    int DispatchDisconnect(ngx_http_request_t *r, nspr_http_response_cb cb);

    int SearchArgs(ngx_str_t a, u_char *k, size_t klen, u_char *v, size_t vlen);

    ngx_chain_t *ParameterError();

}; // class ApiDispatcher


} // namespace nspr

#endif // ifndef __NSPR_API_DISPATCHER_H_INCLUDED__

