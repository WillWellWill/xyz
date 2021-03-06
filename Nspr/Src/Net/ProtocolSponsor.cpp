#include "Nspr.h"

#include "ProtocolSponsor.h"
#include "Metadata.h"
#include "ProtocolConnection.h"
#include "ConnMgmt.h"

namespace nspr
{

ProtocolSponsor::ProtocolSponsor()
    : m_connMgmt(Singleton<ConnMgmt>::GetInstance())
{

}

ProtocolSponsor::~ProtocolSponsor()
{

}

ProtocolConnection *ProtocolSponsor::CreateConnection(const char *url, bool isPublish, ProtocolStatus *status)
{
    u_char connId[1024] = {0};
    GenerateConnId(url, isPublish, connId, sizeof(connId));
    NsprDebug("connId:%s, url:%s", connId, url);
    ProtocolConnection *pc = new ProtocolConnection(connId, (const u_char *)url, status);
    m_connMgmt->AddConnection(connId, pc);
    return pc;
}

int ProtocolSponsor::Disconnect(const u_char *connId)
{
    ProtocolConnection *pc = m_connMgmt->GetConnection(connId);
    if (!pc) {
        return NSPR_ERROR;
    }
    pc->Close();
    return NSPR_OK;
}

void ProtocolSponsor::GenerateConnId(const char *url, bool isPublish, u_char *connId, size_t len)
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    struct tm *lt = localtime(&tv.tv_sec);
    
    char u[1024] = {0};
    char *tmp = strstr((char *)url, "://");
    if (!tmp) {
        tmp = (char *)url;
    }
    else {
        tmp += 3;
    }
    memcpy(u, tmp, strlen(tmp));
    char *ip = &u[0];
    char *port = strchr(ip, ':');
    if (!port) {
        return;
    }
    *port++ = 0;

    char *app = strchr(port, '/');
    if (!app) {
        return;
    }
    *app++ = 0;

    char *liveId = strchr(app, '/');
    if (!liveId) {
        return;
    }
    *liveId++ = 0;

    int ipNum[4] = {0};
    sscanf(ip, "%d.%d.%d.%d", &ipNum[0], &ipNum[1], &ipNum[2], &ipNum[3]);
    snprintf((char *)connId, len, "%s%02d%02d%02d%03ld-%03d%03d%03d%03d-%s-%s"
						, isPublish ? "pu" : "pl"
                        , lt->tm_hour, lt->tm_min, lt->tm_sec, (long)tv.tv_usec / 1000
                        , ipNum[0], ipNum[1], ipNum[2], ipNum[3]
                        , app, liveId);
}

ProtocolConnection *ProtocolSponsor::SearchConnection(const u_char *connId)
{
	return m_connMgmt->GetConnection(connId);
}

int ProtocolSponsor::GetAllConnId(u_char ***connIdArray, int *count)
{
	return m_connMgmt->GetAllConnId(connIdArray, count);
}

} // namespace nspr

