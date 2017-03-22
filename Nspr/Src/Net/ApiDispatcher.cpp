#include "Nspr.h"

#include "ApiDispatcher.h"
#include "MediaSponsor.h"
#include "ProtocolSponsor.h"
#include "ProtocolConnection.h"
#include "LiveSession.h"
#include "WordDb.h"

namespace nspr
{

ApiDispatcher::ApiDispatcher()
{
    ngx_buf_t *b = new ngx_buf_t;
    ngx_memzero(b, sizeof(ngx_buf_t));
    b->pos = b->last = b->start = &m_responseStr[0];
    b->end = b->start + m_maxResponseStrLen;
    b->memory = 1;
    b->last_buf = 1;
    
    m_response = new ngx_chain_t;
    ngx_memzero(m_response, sizeof(ngx_chain_t));
    m_response->buf = b;
}

ApiDispatcher::~ApiDispatcher()
{
    delete m_response->buf;
    delete m_response;
}

int ApiDispatcher::OnRequest(ngx_http_request_t *req, nspr_http_response_cb cb)
{
    const ngx_str_t uri = req->uri;
    if (!ngx_strncmp(uri.data, "/api/publish", sizeof("/api/publish") - 1)) {
        return DispatchPublish(req, cb);
    }
    else if (!ngx_strncmp(uri.data, "/api/play", sizeof("/api/play") - 1)) {
        return DispatchPlay(req, cb);
    }
    else if (!ngx_strncmp(uri.data, "/api/close", sizeof("/api/close") - 1)) {
        return DispatchDisconnect(req, cb);
    }
    else if (!ngx_strncmp(uri.data, "/api/query", sizeof("/api/query") - 1)) {
        return DispatchQuery(req, cb);
    }
    else if (!ngx_strncmp(uri.data, "/api/getanotherword", sizeof("/api/getanotherword") - 1)) {
        return DispatchGetAnotherWord(req, cb);
    }
    return NGX_HTTP_OK;
}
    

int ApiDispatcher::DispatchPublish(ngx_http_request_t *r, nspr_http_response_cb cb)
{
    return DispatchConnect(r, cb, true);
}

int ApiDispatcher::DispatchPlay(ngx_http_request_t *r, nspr_http_response_cb cb)
{
    return DispatchConnect(r, cb, false);
}

int ApiDispatcher::SearchArgs(ngx_str_t a, u_char *k, size_t klen, u_char *v, size_t vlen)
{
    u_char *vb = (u_char *)ngx_strstr(a.data, k),
           *ve = (u_char *)ngx_strchr(a.data, '&');
    if (!ve || ve > a.data + a.len) {
        ve = a.data + a.len;
    }
    vb += klen;
    const int len = ve - vb;
    if (len >= (int)vlen) {
        memcpy(v, vb, vlen - 1);
    }
    else {
        memcpy(v, vb, len);
    }
    return NSPR_OK;
}

int ApiDispatcher::DispatchConnect(ngx_http_request_t *r, nspr_http_response_cb cb, bool isPublish)
{
    u_char url[512] = {0};
    SearchArgs(r->args, (u_char *)"url=", 4, url, sizeof(url));
    
    NsprDebug("receive connect request url:%s", url);
    ProtocolConnection *pc = Singleton<ProtocolSponsor>::GetInstance()->CreateConnection((char *)url, 
							isPublish, new StatusResponse(this, r, cb));
    if (pc->Connect(isPublish)) {
        return NGX_ERROR;
    }
    int len = snprintf((char *)m_responseStr, sizeof(m_responseStr), "{\"connId\":\"%s\"}", (const char *)pc->GetConnId());
    m_response->buf->pos = m_responseStr;
    m_response->buf->last = m_responseStr + len;
    cb(r, m_response);
    return NGX_HTTP_OK;
}

int ApiDispatcher::DispatchGetAnotherWord(ngx_http_request_t *r, nspr_http_response_cb cb)
{
	WordDb wdb;
	u_char *word = wdb.WordRandom();
    int len = snprintf((char *)m_responseStr, sizeof(m_responseStr), "{\"result\":\"%s\"}", word);
    m_response->buf->pos = m_responseStr;
    m_response->buf->last = m_responseStr + len;
    cb(r, m_response);
	return NGX_HTTP_OK;
}

int ApiDispatcher::DispatchQuery(ngx_http_request_t *r, nspr_http_response_cb cb)
{
	u_char **connIdArray;
	int count, i;
    Singleton<ProtocolSponsor>::GetInstance()->GetAllConnId(&connIdArray, &count);
	m_responseStr[0] = '[';
	m_responseStr[1] = '\0';
	for (i = 0; i < count; i++) {
		u_char tmp[512] = {0};
		ProtocolConnection *pc = Singleton<ProtocolSponsor>::GetInstance()->SearchConnection(connIdArray[i]);
		snprintf((char *)tmp, sizeof(tmp), "{\"connId\":\"%s\", \"status\":\"%s\"}", pc->GetConnId(), pc->GetConnSt());
		if (i != 0) {
			strcat((char *)m_responseStr, ",");
		}
		strcat((char *)m_responseStr, (char *)tmp);
	}
	strcat((char *)m_responseStr, "]");
    m_response->buf->pos = m_responseStr;
    m_response->buf->last = m_responseStr + ngx_strlen(m_responseStr);
    cb(r, m_response);
    return NGX_HTTP_OK;
}

int ApiDispatcher::DispatchDisconnect(ngx_http_request_t *r, nspr_http_response_cb cb)
{
    u_char ci[512] = {0};
    SearchArgs(r->args, (u_char *)"connId=", 7, ci, sizeof(ci));
    Singleton<ProtocolSponsor>::GetInstance()->Disconnect(ci);
    int len = snprintf((char *)m_responseStr, sizeof(m_responseStr), "{\"connId\":\"%s\"}", (const char *)ci);
    NsprDebug("to disconnect, connId:%s", (char *)ci);
    m_response->buf->pos = m_responseStr;
    m_response->buf->last = m_responseStr + len;
    cb(r, m_response);
    return NGX_HTTP_OK;
}

StatusResponse::StatusResponse(ApiDispatcher *api, ngx_http_request_t *r, nspr_http_response_cb cb)
    //: m_api(api)
    //, m_r(r)
    //, m_cb(cb)
	: m_liveSession(nullptr)
{
}

void StatusResponse::OnConnected(const u_char *connId, Metadata *metadata)
{
    NsprInfo("connection id:%s connected ok", connId);
	m_liveSession = new LiveSession(connId, metadata);
}

void StatusResponse::OnDestroyed(const u_char *connId)
{
    NsprDebug("connection id:%s disconnected ok", connId);
	if (m_liveSession) {
		delete m_liveSession;
	}

}
void StatusResponse::OnReady(const u_char *connId)
{
	NsprInfo("connection id:%s ready now", connId);
	if (m_liveSession && m_liveSession->CreateStream()) {
		NsprError("create stream failed");
		return;
	}
}

} // namespace nspr
