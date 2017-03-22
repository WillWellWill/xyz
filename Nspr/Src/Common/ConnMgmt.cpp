#include "Nspr.h"

#include "ConnMgmt.h"
#include "ProtocolConnection.h"


namespace nspr
{

ConnMgmt::ConnMgmt()
{
}

ConnMgmt::~ConnMgmt()
{
}

void ConnMgmt::AddConnection(const u_char *connId, ProtocolConnection *pc)
{
    NsprDebug("add %s", (char *)connId);
    m_store[std::string((const char *)connId)] = pc;
}
	
ProtocolConnection *ConnMgmt::GetConnection(const u_char *connId)
{
    std::map<std::string, ProtocolConnection *>::iterator it = m_store.find(std::string((const char *)connId));
    if (it == m_store.end()) {
        NsprWarn("find nothing by %s", (char *)connId);
        return nullptr;
    }
    return it->second;
}

ProtocolConnection *ConnMgmt::DelConnection(const u_char *connId)
{
    std::map<std::string, ProtocolConnection *>::iterator it = m_store.find(std::string((const char *)connId));
    if (it == m_store.end()) {
        NsprWarn("find nothing by %s", (char *)connId);
        return nullptr;
    }
    m_store.erase(it);
    return it->second;
}
	
int ConnMgmt::GetAllConnId(u_char ***connIdArray, int *count)
{
	*count = 0;
	auto it = m_store.begin();
	for (; it != m_store.end(); it++) {
		(*count)++;
	}
	*connIdArray = new u_char*[*count];
	int i = 0;
	for (it = m_store.begin(); it != m_store.end(); it++) {
		(*connIdArray)[i++] = (u_char *)it->first.data();
	}
	
	return NSPR_OK;
}

} // namespace nspr

